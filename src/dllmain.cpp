#include <windows.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <string>
#include <list>
#include <intrin.h>
#include <DirectXMath.h>

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
bool UseRawInput = true;

bool g_keyboardButtonState[256] = {};

std::list<bool(__stdcall*)()> g_registeredWrapCheck;
std::list<RAWKEYBOARD> g_lastKeyboardEvents;
std::list<RAWMOUSE> g_lastMouseEvents;

RAWMOUSE g_lastMouseState;

HWND g_gothicHWND;
WNDPROC g_originalWndProc;
m_IDirectInput7A* g_directInput;
RECT g_clippedRect;

bool g_MultiplyQE = false;
bool g_UseAccumulation = true;
float g_SpeedMultiplierX = 1.0f;
float g_SpeedMultiplierY = 1.0f;
float g_InternalMultiplierX = 1.0f;
float g_InternalMultiplierY = 1.0f;

bool NeedToWarpMouse()
{
	if(!g_registeredWrapCheck.empty())
	{
		for(auto& func : g_registeredWrapCheck)
		{
			if(!func())
				return false;
		}
	}
	if(GetModuleHandleA("UNION_ADV_INVENTORY.DLL"))
	{
		// Handle union advanced inventory
		if(IsG108K)
		{
			DWORD player = *reinterpret_cast<DWORD*>(0x8DBBB0);
			if(player && reinterpret_cast<int(__thiscall*)(DWORD)>(*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(player + 0x550) + 0x1C))(player + 0x550))
				return false;
		}
		else if(IsG2A26F)
		{
			DWORD player = *reinterpret_cast<DWORD*>(0xAB2684);
			if(player && reinterpret_cast<int(__thiscall*)(DWORD)>(*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(player + 0x668) + 0x2C))(player + 0x668))
				return false;
		}
	}
	if(IsG2A26F && GetModuleHandleA("UNION_ABI.DLL"))
	{
		// Handle New Balance crafting menu
		DWORD screen = *reinterpret_cast<DWORD*>(0xAB6468);
		DWORD child = *reinterpret_cast<DWORD*>(screen + 0x38);
		while(child)
		{
			static const char searchChild[] = "AS_CRAFT_CURSOR.TGA";
			if(*reinterpret_cast<int*>(child + 0xB0) && *reinterpret_cast<DWORD*>(child + 0x40))
			{
				DWORD texName = reinterpret_cast<DWORD(__thiscall*)(DWORD)>(0x5A9CD0)(*reinterpret_cast<DWORD*>(child + 0x40));
				if(texName && *reinterpret_cast<int*>(texName + 0x0C) == sizeof(searchChild) - 1)
				{
					if(_stricmp(*reinterpret_cast<const char**>(texName + 0x08), searchChild) == 0)
						return false;
				}
			}
			child = *reinterpret_cast<DWORD*>(child + 0x0C);
		}
	}
	return true;
}

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
	if(msg == WM_INPUT && UseRawInput)
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

			g_lastMouseState = rawMouse;
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
			case VK_CLEAR: new_vk = (extended ? VK_CLEAR : VK_NUMPAD5); break;
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
			case VK_CLEAR: new_vk = (extended ? VK_CLEAR : VK_NUMPAD5); break;
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

	if(!UseRawInput && IsMouseAcquired)
	{
		switch(msg)
		{
			case WM_LBUTTONDOWN:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_1_DOWN;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_LBUTTONUP:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_1_UP;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_RBUTTONDOWN:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_2_DOWN;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_RBUTTONUP:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_2_UP;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_MBUTTONDOWN:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_3_DOWN;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_MBUTTONUP:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_BUTTON_3_UP;

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_XBUTTONDOWN:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = ((GET_XBUTTON_WPARAM(wParam) & XBUTTON2) ? RI_MOUSE_BUTTON_5_DOWN : RI_MOUSE_BUTTON_4_DOWN);

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_XBUTTONUP:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = ((GET_XBUTTON_WPARAM(wParam) & XBUTTON2) ? RI_MOUSE_BUTTON_5_UP : RI_MOUSE_BUTTON_4_UP);

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_MOUSEWHEEL:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_WHEEL;
				rawMouse.usButtonData = GET_WHEEL_DELTA_WPARAM(wParam);

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
			case WM_MOUSEHWHEEL:
			{
				RAWMOUSE rawMouse = {};
				rawMouse.usButtonFlags = RI_MOUSE_HWHEEL;
				rawMouse.usButtonData = GET_WHEEL_DELTA_WPARAM(wParam);

				g_lastMouseState.usButtonFlags = rawMouse.usButtonFlags;
				g_lastMouseEvents.push_back(rawMouse);
			}
			break;
		}
	}
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
		UseRawInput = false;
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

	if(!UseRawInput && IsMouseAcquired && IsWindowActive)
	{
		static int ILastX = 0, ILastY = 0;
		POINT pt = {0, 0};
		GetCursorPos(&pt);
		ClientToScreen(g_gothicHWND, &pt);
		int relativeX = pt.x - ILastX;
		int relativeY = pt.y - ILastY;
		if(NeedToWarpMouse())
		{
			POINT ptNew;
			RECT rect;
			if(GetClipCursor(&rect))
			{
				ptNew.x = rect.left + (rect.right / 2);
				ptNew.y = rect.top + (rect.bottom / 2);
			}
			else
			{
				GetClientRect(g_gothicHWND, &rect);
				ClientToScreen(g_gothicHWND, reinterpret_cast<LPPOINT>(&rect) + 0);
				ClientToScreen(g_gothicHWND, reinterpret_cast<LPPOINT>(&rect) + 1);
				ptNew.x = rect.left + (rect.right / 2);
				ptNew.y = rect.top + (rect.bottom / 2);
			}
			SetCursorPos(ptNew.x, ptNew.y);
			ILastX = ptNew.x;
			ILastY = ptNew.y;
		}
		else
		{
			ILastX = pt.x;
			ILastY = pt.y;
		}

		if(relativeX != 0 || relativeY != 0)
		{
			RAWMOUSE rawMouse = {};
			rawMouse.lLastX = relativeX;
			rawMouse.lLastY = relativeY;

			g_lastMouseState.lLastX = rawMouse.lLastX;
			g_lastMouseState.lLastY = rawMouse.lLastY;
			g_lastMouseEvents.push_back(rawMouse);
		}
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

void __fastcall HookGetMousePos_G2(DWORD zCInput, DWORD _EDX, float& xpos, float& ypos, float& zpos)
{
	xpos = static_cast<float>(*reinterpret_cast<int*>(0x8D165C)) * (*reinterpret_cast<float*>(0x89A148)) * g_InternalMultiplierX;
	ypos = static_cast<float>(*reinterpret_cast<int*>(0x8D1660)) * (*reinterpret_cast<float*>(0x89A14C)) * g_InternalMultiplierY;
	zpos = static_cast<float>(*reinterpret_cast<int*>(0x8D1664));
	if(*reinterpret_cast<int*>(0x8D1D78)) xpos = -xpos;
	if(*reinterpret_cast<int*>(0x8D1D7C)) ypos = -ypos;
}

void __fastcall HookGetMousePos_G1(DWORD zCInput, DWORD _EDX, float& xpos, float& ypos, float& zpos)
{
	xpos = static_cast<float>(*reinterpret_cast<int*>(0x86CCAC)) * (*reinterpret_cast<float*>(0x836538)) * g_InternalMultiplierX;
	ypos = static_cast<float>(*reinterpret_cast<int*>(0x86CCB0)) * (*reinterpret_cast<float*>(0x83653C)) * g_InternalMultiplierY;
	zpos = static_cast<float>(*reinterpret_cast<int*>(0x86CCB4));
	if(*reinterpret_cast<int*>(0x86D304)) xpos = -xpos;
	if(*reinterpret_cast<int*>(0x86D308)) ypos = -ypos;
}

float __fastcall HookReadSmoothMouse(DWORD zCOptions, DWORD _EDX, DWORD section, DWORD option, float defValue)
{
	return 0.0f;
}

int __fastcall HookGetWeaponMode_G1(DWORD oCNpc)
{
	g_MultiplyQE = (reinterpret_cast<int(__thiscall*)(DWORD)>(0x695820)(oCNpc) != 0);
	return 0;
}

float __fastcall MouseRotationFunc_G1(DWORD oCAniCtrl_Human, DWORD _EDX, float angle, int playAnimation)
{
	// Change character rotation function to the G2 equivalent
    float mouseX, mouseY, mouseZ;
    DWORD zCInput = *reinterpret_cast<DWORD*>(0x86CCA0);
    reinterpret_cast<void(__thiscall*)(DWORD, float&, float&, float&)>(*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(zCInput) + 0x50))(zCInput, mouseX, mouseY, mouseZ);
    float rotationScale = *reinterpret_cast<float*>(*reinterpret_cast<DWORD*>(0x614900));
    angle = (mouseX * rotationScale);
    playAnimation = (fabsf(angle) > 1.f);

	float lastFrameTime = *reinterpret_cast<float*>(0x8CF1EC);
	float lastFrameScale = *reinterpret_cast<float*>(0x8CF1E8);
	*reinterpret_cast<float*>(0x8CF1EC) = 16.f; // 16 miliseconds fixed timestep
	*reinterpret_cast<float*>(0x8CF1E8) = 1.f; // disable time scaling
	float res = reinterpret_cast<float(__thiscall*)(DWORD, float, int)>(0x625A30)(oCAniCtrl_Human, angle, playAnimation);
	*reinterpret_cast<float*>(0x8CF1EC) = lastFrameTime;
	*reinterpret_cast<float*>(0x8CF1E8) = lastFrameScale;
	return res;
}

float __fastcall MouseQERotationFunc_G1(DWORD oCAniCtrl_Human, DWORD _EDX, float angle, int playAnimation)
{
	if(g_MultiplyQE) angle *= 2;
	return reinterpret_cast<float(__thiscall*)(DWORD, float, int)>(0x625A30)(oCAniCtrl_Human, angle, playAnimation);
}

float __fastcall MouseRotationFunc_G2(DWORD oCAniCtrl_Human, DWORD _EDX, float angle, int playAnimation)
{
	// Fix character rotating on keyboard q/e with fixed step camera
	angle = (*reinterpret_cast<float*>(0x99B3D8) / (*reinterpret_cast<float*>(0x99B3D4)) * angle) / *reinterpret_cast<float*>(0x57CA85);
	return reinterpret_cast<float(__thiscall*)(DWORD, float, int)>(0x6AE540)(oCAniCtrl_Human, angle, playAnimation);
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

	bool UseFixedStepCameraControl = true;
	{
		char executablePath[MAX_PATH];
		GetModuleFileNameA(GetModuleHandleA(nullptr), executablePath, sizeof(executablePath));
		PathRemoveFileSpecA(executablePath);
		SetCurrentDirectoryA(executablePath);
	}
	{
		std::string currentSector = "none";

		FILE* f;
		errno_t err = fopen_s(&f, "grawinput.ini", "r");
		if(err == 0)
		{
			char readedLine[1024];
			while(fgets(readedLine, sizeof(readedLine), f) != nullptr)
			{
				size_t len = strlen(readedLine);
				if(len > 0)
				{
					if(readedLine[len - 1] == '\n' || readedLine[len - 1] == '\r')
						len -= 1;
					if(len > 0)
					{
						if(readedLine[len - 1] == '\n' || readedLine[len - 1] == '\r')
							len -= 1;
					}
				}
				if(len == 0)
					continue;

				if(readedLine[0] == '[' && readedLine[len - 1] == ']')
				{
					currentSector = std::string(readedLine + 1, len - 2);
					std::transform(currentSector.begin(), currentSector.end(), currentSector.begin(), toupper);
				}
				else if(readedLine[0] != ';' && readedLine[0] != '/')
				{
					std::size_t eqpos;
					std::string rLine = std::string(readedLine, len);
					std::transform(rLine.begin(), rLine.end(), rLine.begin(), toupper);
					if((eqpos = rLine.find("=")) != std::string::npos)
					{
						std::string lhLine = rLine.substr(0, eqpos);
						std::string rhLine = rLine.substr(eqpos + 1);
						lhLine.erase(lhLine.find_last_not_of(' ') + 1);
						lhLine.erase(0, lhLine.find_first_not_of(' '));
						rhLine.erase(rhLine.find_last_not_of(' ') + 1);
						rhLine.erase(0, rhLine.find_first_not_of(' '));
						if(currentSector == "MOUSE")
						{
							if(lhLine == "USEACCUMULATION")
								g_UseAccumulation = (rhLine == "TRUE" || rhLine == "1");
							else if(lhLine == "USEFIXEDSTEPCAMERACONTROL")
								UseFixedStepCameraControl = (rhLine == "TRUE" || rhLine == "1");
							else if(lhLine == "SPEEDMULTIPLIERX")
							{
								try {g_SpeedMultiplierX = std::stof(rhLine);}
								catch(const std::exception&) {g_SpeedMultiplierX = 1.0f;}
							}
							else if(lhLine == "SPEEDMULTIPLIERY")
							{
								try {g_SpeedMultiplierY = std::stof(rhLine);}
								catch(const std::exception&) {g_SpeedMultiplierY = 1.0f;}
							}
						}
						else if(currentSector == "KEYBOARD")
						{
							auto vit = VK_Keys.find(lhLine);
							auto dit = DIK_Keys.find(rhLine);
							if(vit == VK_Keys.end() || dit == DIK_Keys.end())
							{
								std::string text = "Not found keyboard identifier: ";
								if(vit == VK_Keys.end()) text.append(lhLine);
								else text.append(rhLine);
								MessageBoxA(nullptr, text.c_str(), "GRawInput", MB_ICONWARNING);
							}
							else
								VK_Keys_Map.emplace(vit->second, static_cast<DWORD>(dit->second));
						}
					}
				}
			}
			fclose(f);

			VK_Keys.clear();
			DIK_Keys.clear();
		}
	}

	if(IsG2A26F && g_UseAccumulation)
	{
		WriteStack(0x4D3E50, "\x01");
		WriteStack(0x4D3E5C, "\x01");
	}
	else if(IsG108K && g_UseAccumulation)
	{
		WriteStack(0x4C7FEB, "\x01");
		WriteStack(0x4C7FF7, "\x01");
	}

	if(IsG2A26F && UseFixedStepCameraControl)
	{
		OverWriteFloat(0x57CA85, 16.f);
		OverWriteFloat(0x57CA89, 0.016f);
		WriteStack(0x699D97, "\xD9\x05\x85\xCA\x57\x00\x90\x90\x90\x90\x90\x90");
		WriteStack(0x6AE84B, "\x85\xCA\x57\x00");
		WriteStack(0x6AE86C, "\x90\x90\x90\x90");
		WriteStack(0x4A4D25, "\xD9\x05\x89\xCA\x57\x00\x90\x90\x90");
		WriteStack(0x4A4DDC, "\xD9\x05\x89\xCA\x57\x00\x90\x90\x90");
		WriteStack(0x4A4E61, "\xD9\x05\x89\xCA\x57\x00\x90\x90\x90");

		HookCall(0x69AAA5, reinterpret_cast<DWORD>(&MouseRotationFunc_G2));
		HookCall(0x69AAE1, reinterpret_cast<DWORD>(&MouseRotationFunc_G2));
		HookCall(0x69ABA3, reinterpret_cast<DWORD>(&MouseRotationFunc_G2));
		HookCall(0x69ABDB, reinterpret_cast<DWORD>(&MouseRotationFunc_G2));
	}
	else if(IsG108K && UseFixedStepCameraControl)
	{
		OverWriteFloat(0x4266C4, 0.5f);
		OverWriteFloat(0x4266C8, 0.3f);
		OverWriteFloat(0x562CC5, 16.f);
		OverWriteFloat(0x562CC9, 0.016f);
		WriteStack(0x426664, "\xD8\x05\xC8\x66\x42\x00");
		WriteStack(0x426CBB, "\xD8\x05\xC8\x66\x42\x00");
		WriteStack(0x426656, "\xD8\x0D\xC4\x66\x42\x00");
		WriteStack(0x426CAD, "\xD8\x0D\xC4\x66\x42\x00");
		WriteStack(0x613711, "\xD9\x05\xC5\x2C\x56\x00");
		WriteStack(0x49D428, "\xD9\x05\xC9\x2C\x56\x00\x90\x90\x90");
		WriteStack(0x49D4E3, "\xD9\x05\xC9\x2C\x56\x00\x90\x90\x90");
		WriteStack(0x49D568, "\xD9\x05\xC9\x2C\x56\x00\x90\x90\x90");

		HookCall(0x614858, reinterpret_cast<DWORD>(&HookGetWeaponMode_G1));
		HookCall(0x6148B8, reinterpret_cast<DWORD>(&MouseQERotationFunc_G1));
		HookCall(0x6148E9, reinterpret_cast<DWORD>(&MouseQERotationFunc_G1));
		HookCall(0x61490C, reinterpret_cast<DWORD>(&MouseRotationFunc_G1));
		HookCall(0x614A17, reinterpret_cast<DWORD>(&MouseRotationFunc_G1));
		WriteStack(0x614874, "\xEB\x0A");
		WriteStack(0x61488F, "\xEB\x08");
		Nop(0x614907, 2);
		Nop(0x614A12, 2);
	}

	if(IsG2A26F)
	{
		g_InternalMultiplierX = g_SpeedMultiplierX;
		g_InternalMultiplierY = g_SpeedMultiplierY;
		g_SpeedMultiplierX = 1.0f;
		g_SpeedMultiplierY = 1.0f;
		HookJMP(0x4D5730, reinterpret_cast<DWORD>(&HookGetMousePos_G2));
		HookCall(0x4D414F, reinterpret_cast<DWORD>(&HookReadSmoothMouse));
	}
	else if(IsG108K)
	{
		g_InternalMultiplierX = g_SpeedMultiplierX;
		g_InternalMultiplierY = g_SpeedMultiplierY;
		g_SpeedMultiplierX = 1.0f;
		g_SpeedMultiplierY = 1.0f;
		HookJMP(0x4C8BD0, reinterpret_cast<DWORD>(&HookGetMousePos_G1));
	}

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

	HRESULT WINAPI _RegisterWrapCheck(bool(__stdcall* wrapCheckFunction)())
	{
		g_registeredWrapCheck.emplace_back(wrapCheckFunction);
		return DI_OK;
	}

	HRESULT WINAPI _DInputToRawInput()
	{
		return DI_OK;
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

