#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

DWORD addressMessageBox = 0;
char oldMessageBoxBytes[5];
void* g_directInput;

int WINAPI Hooked_MessageBoxA(HWND hWnd, const char* lpText, const char* lpCaption, UINT uType)
{
	DWORD dwOldProtect, dwNewProtect;
	VirtualProtect(reinterpret_cast<LPVOID>(addressMessageBox), 5, PAGE_READWRITE, &dwOldProtect);
	memcpy(reinterpret_cast<LPVOID>(addressMessageBox), oldMessageBoxBytes, 5);
	VirtualProtect(reinterpret_cast<LPVOID>(addressMessageBox), 5, dwOldProtect, &dwNewProtect);
	return 0;
}

extern "C"
{
	HRESULT WINAPI _DirectInput8Create(HINSTANCE, DWORD, REFIID, LPVOID* lplpDD, LPUNKNOWN)
	{
		if(addressMessageBox == 0)
		{
			HMODULE user32dll = GetModuleHandleA("user32.dll");
			if(user32dll)
			{
				addressMessageBox = reinterpret_cast<DWORD>(GetProcAddress(user32dll, "MessageBoxA"));
				if(addressMessageBox == 0)
				{
					g_directInput = nullptr;
					*lplpDD = g_directInput;
					return DIERR_GENERIC;
				}
			}
			else
			{
				g_directInput = nullptr;
				*lplpDD = g_directInput;
				return DIERR_GENERIC;
			}
		}

		DWORD dwOldProtect, dwNewProtect, dwNewCall;
		dwNewCall = reinterpret_cast<DWORD>(&Hooked_MessageBoxA) - addressMessageBox - 5;
		VirtualProtect(reinterpret_cast<LPVOID>(addressMessageBox), 5, PAGE_READWRITE, &dwOldProtect);
		memcpy(oldMessageBoxBytes, reinterpret_cast<LPVOID>(addressMessageBox), 5);
		*reinterpret_cast<BYTE*>(addressMessageBox) = 0xE9;
		*reinterpret_cast<DWORD*>(addressMessageBox + 1) = dwNewCall;
		VirtualProtect(reinterpret_cast<LPVOID>(addressMessageBox), 5, dwOldProtect, &dwNewProtect);

		g_directInput = nullptr;
		*lplpDD = g_directInput;
		return DIERR_GENERIC;
	}

	HRESULT WINAPI _DllCanUnloadNow()
	{
		return DI_OK;
	}

	HRESULT WINAPI _DllGetClassObject(IN REFCLSID rclsid, IN REFIID, OUT LPVOID FAR* ppv)
	{
		HRESULT hr = DI_OK;
		if(rclsid == CLSID_DirectInput8)
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
			case DLL_THREAD_ATTACH:
			case DLL_THREAD_DETACH:
			case DLL_PROCESS_DETACH:
				break;
		}
		return 1;
	}
}
