#include <windows.h>
#include <shlwapi.h>
#include <math.h>
#include <list>

#include "dinput/IDirectInput7A.h"
#include "keymapping.h"

#include "hook.h"

#pragma comment(lib, "shlwapi.lib")

bool IsG108K = false;
bool IsG112F = false;
bool IsG2A26F = false;

bool IsEnabledUnionWrapper = false;
bool IsRegistered = false;
bool IsKeyboardAcquired = false;
bool IsMouseAcquired = false;
bool IsJoystickAcquired = false;
bool IsWindowActive = true;

bool g_keyboardButtonState[256] = {};

std::list<RAWKEYBOARD> g_lastKeyboardEvents;
std::list<RAWMOUSE> g_lastMouseEvents;

HWND g_gothicHWND;
WNDPROC g_originalWndProc;
m_IDirectInput7A* g_directInput;
RECT g_clippedRect;

void ClearKeyBuffer()
{
	for(int i = 0; i < 256; ++i)
	{
		if(g_keyboardButtonState[i])
		{
			for(auto& it : VK_Keys_Map)
			{
				if(it.second == i)
				{
					RAWKEYBOARD keyboardKey;
					keyboardKey.Message = WM_KEYUP;
					keyboardKey.VKey = static_cast<USHORT>(it.first);
					g_lastKeyboardEvents.push_back(keyboardKey);
					break;
				}
			}
			g_keyboardButtonState[i] = false;
		}
	}
}

void UpdateClipCursor(HWND hwnd)
{
	RECT rect;
	if(IsWindowActive)
	{
		GetClientRect(hwnd, &rect);
		ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&rect) + 0);
		ClientToScreen(hwnd, reinterpret_cast<LPPOINT>(&rect) + 1);
		if(ClipCursor(&rect))
			g_clippedRect = rect;
	}
	else
	{
		if(GetClipCursor(&rect) && memcmp(&rect, &g_clippedRect, sizeof(RECT)) == 0)
		{
			ClipCursor(nullptr);
			ZeroMemory(&g_clippedRect, sizeof(RECT));
		}
	}
}

LRESULT CALLBACK RawInputWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_INPUT)
	{
		UINT dwSize = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		LPBYTE rawData = new BYTE[dwSize];
		if(rawData == NULL)
			return 0;

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* rinput = reinterpret_cast<RAWINPUT*>(rawData);
		if(rinput->header.dwType == RIM_TYPEMOUSE && IsMouseAcquired)
		{
			RAWMOUSE& rawMouse = rinput->data.mouse;
			g_lastMouseEvents.push_back(rawMouse);
		}
	}
	else if((msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) && IsKeyboardAcquired)
	{
		WPARAM new_vk = wParam;
		bool extended = (lParam & 0x01000000) != 0;
		switch(new_vk)
		{
			case VK_SHIFT: new_vk = MapVirtualKeyA((lParam & 0x00FF0000) >> 16, MAPVK_VSC_TO_VK_EX); break;
			case VK_CONTROL: new_vk = (extended ? VK_RCONTROL : VK_LCONTROL); break;
			case VK_MENU: new_vk = (extended ? VK_RMENU : VK_LMENU); break;
			case VK_RETURN: new_vk = (extended ? 0xE8 : VK_RETURN); break;
			case VK_INSERT: new_vk = (extended ? VK_INSERT : VK_NUMPAD0); break;
			case VK_END: new_vk = (extended ? VK_END : VK_NUMPAD1); break;
			case VK_DOWN: new_vk = (extended ? VK_DOWN : VK_NUMPAD2); break;
			case VK_NEXT: new_vk = (extended ? VK_NEXT : VK_NUMPAD3); break;
			case VK_LEFT: new_vk = (extended ? VK_LEFT : VK_NUMPAD4); break;
			case VK_RIGHT: new_vk = (extended ? VK_RIGHT : VK_NUMPAD6); break;
			case VK_HOME: new_vk = (extended ? VK_HOME : VK_NUMPAD7); break;
			case VK_UP: new_vk = (extended ? VK_UP : VK_NUMPAD8); break;
			case VK_PRIOR: new_vk = (extended ? VK_PRIOR : VK_NUMPAD9); break;
			case VK_DELETE: new_vk = (extended ? VK_DELETE : VK_DECIMAL); break;
		}

		RAWKEYBOARD keyboardKey;
		keyboardKey.Message = WM_KEYDOWN;
		keyboardKey.VKey = static_cast<USHORT>(new_vk);
		g_lastKeyboardEvents.push_back(keyboardKey);
	}
	else if((msg == WM_KEYUP || msg == WM_SYSKEYUP) && IsKeyboardAcquired)
	{
		WPARAM new_vk = wParam;
		bool extended = (lParam & 0x01000000) != 0;
		switch(new_vk)
		{
			case VK_SHIFT: new_vk = MapVirtualKeyA((lParam & 0x00FF0000) >> 16, MAPVK_VSC_TO_VK_EX); break;
			case VK_CONTROL: new_vk = (extended ? VK_RCONTROL : VK_LCONTROL); break;
			case VK_MENU: new_vk = (extended ? VK_RMENU : VK_LMENU); break;
			case VK_RETURN: new_vk = (extended ? 0xE8 : VK_RETURN); break;
			case VK_INSERT: new_vk = (extended ? VK_INSERT : VK_NUMPAD0); break;
			case VK_END: new_vk = (extended ? VK_END : VK_NUMPAD1); break;
			case VK_DOWN: new_vk = (extended ? VK_DOWN : VK_NUMPAD2); break;
			case VK_NEXT: new_vk = (extended ? VK_NEXT : VK_NUMPAD3); break;
			case VK_LEFT: new_vk = (extended ? VK_LEFT : VK_NUMPAD4); break;
			case VK_RIGHT: new_vk = (extended ? VK_RIGHT : VK_NUMPAD6); break;
			case VK_HOME: new_vk = (extended ? VK_HOME : VK_NUMPAD7); break;
			case VK_UP: new_vk = (extended ? VK_UP : VK_NUMPAD8); break;
			case VK_PRIOR: new_vk = (extended ? VK_PRIOR : VK_NUMPAD9); break;
			case VK_DELETE: new_vk = (extended ? VK_DELETE : VK_DECIMAL); break;
		}

		RAWKEYBOARD keyboardKey;
		keyboardKey.Message = WM_KEYUP;
		keyboardKey.VKey = static_cast<USHORT>(new_vk);
		g_lastKeyboardEvents.push_back(keyboardKey);
	}
	else if(msg == WM_ACTIVATE)
	{
		// Don't mark the window as active if it's activated before being shown
		if(IsWindowVisible(hwnd))
		{
			BOOL minimized = HIWORD(wParam);
			if(!minimized && LOWORD(wParam) != WA_INACTIVE)
				IsWindowActive = true;
			else
				IsWindowActive = false;

			UpdateClipCursor(hwnd);
			ClearKeyBuffer();
		}
	}
	else if(msg == WM_WINDOWPOSCHANGED)
		UpdateClipCursor(hwnd);

	return CallWindowProcA(g_originalWndProc, hwnd, msg, wParam, lParam);
}

void InitRawInput(HWND hwnd)
{
	if(IsRegistered)
		return;

	g_gothicHWND = hwnd;
	g_originalWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(hwnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(RawInputWndProc)));
	IsRegistered = true;
}

void InitMouseRawInput(HWND hwnd)
{
	InitRawInput(hwnd);
}

void AcquireMouseInput()
{
	if(!IsMouseAcquired)
	{
		while(ShowCursor(false) >= 0);
	}
	IsMouseAcquired = true;
}

void UnAcquireMouseInput()
{
	if(IsMouseAcquired)
	{
		while(ShowCursor(true) < 0);
	}
	IsMouseAcquired = false;
}

void InitKeyboardRawInput(HWND hwnd)
{
	InitRawInput(hwnd);
	UpdateClipCursor(hwnd);

	RAWINPUTDEVICE Rid;
	Rid.usUsagePage = 0x01;
	Rid.usUsage = 0x02;
	Rid.dwFlags = 0;
	Rid.hwndTarget = hwnd;
	if(RegisterRawInputDevices(&Rid, 1, sizeof(Rid)) == FALSE)
	{
		MessageBoxA(nullptr, "Mouse Raw Input registration failed.", "Raw Input Error", (MB_ICONHAND|MB_OK));
		exit(-1);
	}
}

void AcquireKeyboardInput()
{
	IsKeyboardAcquired = true;
}

void UnAcquireKeyboardInput()
{
	IsKeyboardAcquired = false;
}

void ProcessInputEvents()
{
	// Make sure we got window messages readed
	MSG msg;
	while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	if(IsWindowActive)
	{
		RECT rect;
		if(GetClipCursor(&rect) && memcmp(&rect, &g_clippedRect, sizeof(RECT)) != 0)
			UpdateClipCursor(g_gothicHWND);
	}
	else if(GetForegroundWindow() == g_gothicHWND)
	{
		// Just in case to check if somehow we didn't get informed the window got activated
		IsWindowActive = true;
		UpdateClipCursor(g_gothicHWND);
	}
}

void CheckUnionWrapper()
{
	static bool s_firstTime = true;
	if(s_firstTime)
	{
		if(!IsEnabledUnionWrapper)
		{
			if(IsG108K && *reinterpret_cast<DWORD*>(0x4C7C11) == 0x90909090)
				IsEnabledUnionWrapper = true;
			else if(IsG112F && *reinterpret_cast<DWORD*>(0x4D776E) == 0x90909090)
				IsEnabledUnionWrapper = true;
			else if(IsG2A26F && *reinterpret_cast<DWORD*>(0x4D377A) == 0x90909090)
				IsEnabledUnionWrapper = true;
			else
			{
				char iniPath[MAX_PATH];
				GetModuleFileNameA(GetModuleHandleA(nullptr), iniPath, sizeof(iniPath));
				PathRemoveFileSpecA(iniPath);
				strcat_s(iniPath, MAX_PATH, "\\SystemPack.ini");

				char buffer[MAX_PATH];
				GetPrivateProfileStringA("MOUSE", "EnableWrapper", "", buffer, MAX_PATH, iniPath);
				if(_stricmp(buffer, "1") == 0 || _stricmp(buffer, "true") == 0)
					IsEnabledUnionWrapper = true;
			}
		}
		if(IsEnabledUnionWrapper)
			MessageBoxA(nullptr, "Detected Union Mouse Wrapper\nRaw Input will be launched in compatibility mode which is not recommended.", "Raw Input Warning", (MB_ICONHAND|MB_OK));

		s_firstTime = false;
	}
}

static int Init()
{
	DWORD baseAddr = reinterpret_cast<DWORD>(GetModuleHandleA(nullptr));

	// Check for gothic 2.6 fix
	if(*reinterpret_cast<DWORD*>(baseAddr + 0x168) == 0x3D4318 && *reinterpret_cast<DWORD*>(baseAddr + 0x3D43A0) == 0x82E108 && *reinterpret_cast<DWORD*>(baseAddr + 0x3D43CB) == 0x82E10C)
		IsG2A26F = true;

	// Check for gothic 1.12F
	if(*reinterpret_cast<DWORD*>(baseAddr + 0x140) == 0x3BE698 && *reinterpret_cast<DWORD*>(baseAddr + 0x3BE720) == 0x8131E4 && *reinterpret_cast<DWORD*>(baseAddr + 0x3BE74B) == 0x8131E8)
		IsG112F = true;

	// Check for gothic 1.08K
	if(*reinterpret_cast<DWORD*>(baseAddr + 0x160) == 0x37A8D8 && *reinterpret_cast<DWORD*>(baseAddr + 0x37A960) == 0x7D01E4 && *reinterpret_cast<DWORD*>(baseAddr + 0x37A98B) == 0x7D01E8)
		IsG108K = true;

	return 1;
}

extern "C"
{
	HRESULT WINAPI _DirectInputCreateEx(HINSTANCE, DWORD, REFIID, LPVOID* lplpDD, LPUNKNOWN)
	{
		g_directInput = new m_IDirectInput7A();
		*lplpDD = g_directInput;
		return DI_OK;
	}

	HRESULT WINAPI _DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* lplpDirectInput, LPUNKNOWN punkOuter)
	{
		return _DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputA, (LPVOID*)lplpDirectInput, punkOuter);
	}

	HRESULT WINAPI _DirectInputCreateW(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTW* lplpDirectInput, LPUNKNOWN punkOuter)
	{
		return _DirectInputCreateEx(hinst, dwVersion, IID_IDirectInputW, (LPVOID*)lplpDirectInput, punkOuter);
	}

	HRESULT WINAPI _DllCanUnloadNow()
	{
		return DI_OK;
	}

	HRESULT WINAPI _DllGetClassObject(IN REFCLSID rclsid, IN REFIID, OUT LPVOID FAR* ppv)
	{
		HRESULT hr = DI_OK;
		if(rclsid == CLSID_DirectInput)
			*ppv = g_directInput;
		else
			hr = DIERR_GENERIC;

		return hr;
	}

	HRESULT WINAPI _DllRegisterServer()
	{
		return DI_OK;
	}

	HRESULT WINAPI _DllUnregisterServer()
	{
		return DI_OK;
	}
}

extern "C"
{
	BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
	{
		switch(dwReason)
		{
			case DLL_PROCESS_ATTACH:
				return Init();

			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				break;
		}
		return 1;
	}
}

