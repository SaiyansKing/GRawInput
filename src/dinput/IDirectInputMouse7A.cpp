#include "IDirectInput7A.h"
#include "IDirectInputMouse7A.h"

#include <list>
#include <algorithm>

extern bool IsMouseAcquired;
extern std::list<RAWMOUSE> g_lastMouseEvents;
std::list<DIDEVICEOBJECTDATA> g_generatedMouseEvents;

extern RAWMOUSE g_lastMouseState;

extern bool g_UseAccumulation;
extern float g_SpeedMultiplierX;
extern float g_SpeedMultiplierY;

HRESULT m_IDirectInputMouse7A::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	(void)riid;
	(void)ppvObj;

	return DIERR_GENERIC;
}

ULONG m_IDirectInputMouse7A::AddRef()
{
	return ++m_references;
}

ULONG m_IDirectInputMouse7A::Release()
{
	ULONG ref = --m_references;
	if(ref == 0) delete this;
	return ref;
}

HRESULT m_IDirectInputMouse7A::GetCapabilities(LPDIDEVCAPS)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetProperty(REFGUID, LPDIPROPHEADER)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SetProperty(REFGUID, LPCDIPROPHEADER)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::Acquire()
{
	void AcquireMouseInput(); AcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::Unacquire()
{
	void UnAcquireMouseInput(); UnAcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetDeviceState(DWORD size, LPVOID lpDeviceState)
{
	if (!IsMouseAcquired)
		return DIERR_NOTACQUIRED;

	DIMOUSESTATE2* deviceStateOut = reinterpret_cast<DIMOUSESTATE2*>(lpDeviceState);

	LONG relativeX = 0;
	LONG relativeY = 0;

	if (g_lastMouseState.lLastX != 0)
		relativeX = (g_UseAccumulation ? relativeX : 0) + g_lastMouseState.lLastX;
	if (g_lastMouseState.lLastY != 0)
		relativeY = (g_UseAccumulation ? relativeY : 0) + g_lastMouseState.lLastY;

	float scrollDelta = 0.0f;

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL))
		scrollDelta = static_cast<float>(static_cast<short>(g_lastMouseState.usButtonData));

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_1_UP))
		deviceStateOut->rgbButtons[0] = (g_lastMouseState.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN ? 0x80 : 0);

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_2_UP))
		deviceStateOut->rgbButtons[1] = (g_lastMouseState.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN ? 0x80 : 0);

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_3_UP))
		deviceStateOut->rgbButtons[2] = (g_lastMouseState.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN ? 0x80 : 0);

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP))
		deviceStateOut->rgbButtons[3] = (g_lastMouseState.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN ? 0x80 : 0);

	if (g_lastMouseState.usButtonFlags & (RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP))
		deviceStateOut->rgbButtons[4] = (g_lastMouseState.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN ? 0x80 : 0);

	deviceStateOut->lX = static_cast<DWORD>(static_cast<LONG>(relativeX * g_SpeedMultiplierX));
	deviceStateOut->lY = static_cast<DWORD>(static_cast<LONG>(relativeY * g_SpeedMultiplierY));
	deviceStateOut->lZ = static_cast<LONG>(scrollDelta > 0 ? ceilf(scrollDelta) : floorf(scrollDelta));

	memset(&g_lastMouseState, 0, sizeof(RAWMOUSE));

	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD)
{
	if(!IsMouseAcquired)
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	if(!g_lastMouseEvents.empty())
	{
		LONG relativeX = 0;
		LONG relativeY = 0;
		do
		{
			RAWMOUSE& rawMouse = g_lastMouseEvents.front();
			if(rawMouse.lLastX != 0)
				relativeX = (g_UseAccumulation ? relativeX : 0) + rawMouse.lLastX;
			if(rawMouse.lLastY != 0)
				relativeY = (g_UseAccumulation ? relativeY : 0) + rawMouse.lLastY;
			if(rawMouse.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL))
			{
				float wheelDelta = static_cast<float>(static_cast<short>(rawMouse.usButtonData));
				float numTicks = wheelDelta / WHEEL_DELTA;

				bool isHorizontalScroll = (rawMouse.usButtonFlags & RI_MOUSE_HWHEEL) == RI_MOUSE_HWHEEL;
				bool isScrollByPage = false;
				float scrollDelta = numTicks;
				if(isHorizontalScroll)
				{
					unsigned long scrollChars = 1;
					SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &scrollChars, 0);
					scrollDelta *= scrollChars;
				}
				else
				{
					unsigned long scrollLines = 3;
					SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
					if(scrollLines == WHEEL_PAGESCROLL)
						isScrollByPage = true;
					else
						scrollDelta *= scrollLines;
				}

				DIDEVICEOBJECTDATA zMotion;
				zMotion.dwOfs = DIMOFS_Z;
				zMotion.dwData = static_cast<DWORD>(static_cast<LONG>(scrollDelta > 0 ? ceilf(scrollDelta) : floorf(scrollDelta)));
				g_generatedMouseEvents.push_back(zMotion);
			}
			if(rawMouse.usButtonFlags & (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_1_UP))
			{
				DIDEVICEOBJECTDATA mouseButton;
				mouseButton.dwOfs = DIMOFS_BUTTON0;
				mouseButton.dwData = (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN ? 0x80 : 0);
				g_generatedMouseEvents.push_back(mouseButton);
			}
			if(rawMouse.usButtonFlags & (RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_2_UP))
			{
				DIDEVICEOBJECTDATA mouseButton;
				mouseButton.dwOfs = DIMOFS_BUTTON1;
				mouseButton.dwData = (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN ? 0x80 : 0);
				g_generatedMouseEvents.push_back(mouseButton);
			}
			if(rawMouse.usButtonFlags & (RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_3_UP))
			{
				DIDEVICEOBJECTDATA mouseButton;
				mouseButton.dwOfs = DIMOFS_BUTTON2;
				mouseButton.dwData = (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN ? 0x80 : 0);
				g_generatedMouseEvents.push_back(mouseButton);
			}
			if(rawMouse.usButtonFlags & (RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP))
			{
				DIDEVICEOBJECTDATA mouseButton;
				mouseButton.dwOfs = DIMOFS_BUTTON3;
				mouseButton.dwData = (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN ? 0x80 : 0);
				g_generatedMouseEvents.push_back(mouseButton);
			}
			if(rawMouse.usButtonFlags & (RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP))
			{
				DIDEVICEOBJECTDATA mouseButton;
				mouseButton.dwOfs = DIMOFS_BUTTON4;
				mouseButton.dwData = (rawMouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN ? 0x80 : 0);
				g_generatedMouseEvents.push_back(mouseButton);
			}
			g_lastMouseEvents.pop_front();
		} while(!g_lastMouseEvents.empty());

		if(relativeX != 0)
		{
			DIDEVICEOBJECTDATA xMotion;
			xMotion.dwOfs = DIMOFS_X;
			xMotion.dwData = static_cast<DWORD>(static_cast<LONG>(relativeX * g_SpeedMultiplierX));
			g_generatedMouseEvents.push_back(xMotion);
		}
		if(relativeY != 0)
		{
			DIDEVICEOBJECTDATA yMotion;
			yMotion.dwOfs = DIMOFS_Y;
			yMotion.dwData = static_cast<DWORD>(static_cast<LONG>(relativeY * g_SpeedMultiplierY));
			g_generatedMouseEvents.push_back(yMotion);
		}
	}

	if(!rgdod)
	{
		*pdwInOut = static_cast<DWORD>(g_generatedMouseEvents.size());
		return DI_OK;
	}
	if(*pdwInOut > 0 && !g_generatedMouseEvents.empty())
	{
		*pdwInOut = std::min<DWORD>(*pdwInOut, g_generatedMouseEvents.size());
		for(DWORD i = 0; i < *pdwInOut; ++i)
		{
			CopyMemory(reinterpret_cast<void*>(reinterpret_cast<DWORD>(rgdod) + (cbObjectData * i)), &g_generatedMouseEvents.front(), cbObjectData);
			g_generatedMouseEvents.pop_front();
		}
		return DI_OK;
	}
	*pdwInOut = 0;
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SetDataFormat(LPCDIDATAFORMAT)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SetEventNotification(HANDLE)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	void InitMouseRawInput(HWND hwnd); InitMouseRawInput(hwnd);
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetDeviceInfo(LPDIDEVICEINSTANCEA)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::RunControlPanel(HWND, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::Initialize(HINSTANCE, DWORD, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT*, LPUNKNOWN)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetEffectInfo(LPDIEFFECTINFOA, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::GetForceFeedbackState(LPDWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SendForceFeedbackCommand(DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::Escape(LPDIEFFESCAPE)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::Poll()
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::EnumEffectsInFile(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouse7A::WriteEffectToFile(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD)
{
	// Not used
	return DI_OK;
}
