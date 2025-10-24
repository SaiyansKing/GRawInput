#include "IDirectInput7A.h"
#include "IDirectInputKeyboard7A.h"
#include "../keymapping.h"

#include <list>
#include <map>

extern bool IsKeyboardAcquired;
extern bool g_keyboardButtonState[256];
extern std::list<RAWKEYBOARD> g_lastKeyboardEvents;
std::list<DIDEVICEOBJECTDATA> g_generatedKeyboardEvents;

extern CRITICAL_SECTION g_criticalSection;

HRESULT m_IDirectInputKeyboard7A::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	(void)riid;
	(void)ppvObj;

	return DIERR_GENERIC;
}

ULONG m_IDirectInputKeyboard7A::AddRef()
{
	return ++m_references;
}

ULONG m_IDirectInputKeyboard7A::Release()
{
	ULONG ref = --m_references;
	if(ref == 0) delete this;
	return ref;
}

HRESULT m_IDirectInputKeyboard7A::GetCapabilities(LPDIDEVCAPS)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetProperty(REFGUID, LPDIPROPHEADER)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SetProperty(REFGUID, LPCDIPROPHEADER)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::Acquire()
{
	void AcquireKeyboardInput(); AcquireKeyboardInput();
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::Unacquire()
{
	void UnAcquireKeyboardInput(); UnAcquireKeyboardInput();
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetDeviceState(DWORD, LPVOID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD)
{
	if(!IsKeyboardAcquired)
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	if(!g_lastKeyboardEvents.empty())
	{
		EnterCriticalSection(&g_criticalSection);

		do
		{
			RAWKEYBOARD& rawKeyboard = g_lastKeyboardEvents.front();
			DWORD keyId = VK_Keys_Map[rawKeyboard.VKey];
			if(keyId != 0)
			{
				if(rawKeyboard.Message == WM_KEYDOWN && !g_keyboardButtonState[keyId])
				{
					DIDEVICEOBJECTDATA keyboardKey;
					keyboardKey.dwOfs = keyId;
					keyboardKey.dwData = 0x80;
					g_generatedKeyboardEvents.push_back(keyboardKey);
					g_keyboardButtonState[keyId] = true;
				}
				else if(rawKeyboard.Message == WM_KEYUP)
				{
					DIDEVICEOBJECTDATA keyboardKey;
					keyboardKey.dwOfs = keyId;
					keyboardKey.dwData = 0;
					g_generatedKeyboardEvents.push_back(keyboardKey);
					g_keyboardButtonState[keyId] = false;
				}
			}
			g_lastKeyboardEvents.pop_front();
		} while(!g_lastKeyboardEvents.empty());

		LeaveCriticalSection(&g_criticalSection);
	}
	
	if(!rgdod)
	{
		*pdwInOut = static_cast<DWORD>(g_generatedKeyboardEvents.size());
		return DI_OK;
	}
	if(*pdwInOut > 0 && !g_generatedKeyboardEvents.empty())
	{
		*pdwInOut = std::min<DWORD>(*pdwInOut, g_generatedKeyboardEvents.size());
		for(DWORD i = 0; i < *pdwInOut; ++i)
		{
			CopyMemory(reinterpret_cast<void*>(reinterpret_cast<DWORD>(rgdod) + (cbObjectData * i)), &g_generatedKeyboardEvents.front(), cbObjectData);
			g_generatedKeyboardEvents.pop_front();
		}
		return DI_OK;
	}
	*pdwInOut = 0;
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SetDataFormat(LPCDIDATAFORMAT)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SetEventNotification(HANDLE)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	void InitKeyboardRawInput(HWND hwnd); InitKeyboardRawInput(hwnd);
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetDeviceInfo(LPDIDEVICEINSTANCEA)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::RunControlPanel(HWND, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::Initialize(HINSTANCE, DWORD, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT*, LPUNKNOWN)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetEffectInfo(LPDIEFFECTINFOA, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::GetForceFeedbackState(LPDWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SendForceFeedbackCommand(DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::Escape(LPDIEFFESCAPE)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::Poll()
{
	void ProcessInputEvents(); ProcessInputEvents();
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::EnumEffectsInFile(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputKeyboard7A::WriteEffectToFile(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD)
{
	// Not used
	return DI_OK;
}
