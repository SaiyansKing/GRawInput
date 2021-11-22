#include "IDirectInput7A.h"
#include "IDirectInputMouseUnion7A.h"

#include <list>
#include <algorithm>

extern bool IsMouseAcquired;
extern std::list<RAWMOUSE> g_lastMouseEvents;
extern std::list<DIDEVICEOBJECTDATA> g_generatedMouseEvents;

HRESULT m_IDirectInputMouseUnion7A::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	(void)riid;
	(void)ppvObj;

	return DIERR_GENERIC;
}

ULONG m_IDirectInputMouseUnion7A::AddRef()
{
	return ++m_references;
}

ULONG m_IDirectInputMouseUnion7A::Release()
{
	ULONG ref = --m_references;
	if(ref == 0) delete this;
	return ref;
}

HRESULT m_IDirectInputMouseUnion7A::GetCapabilities(LPDIDEVCAPS)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetProperty(REFGUID, LPDIPROPHEADER)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::SetProperty(REFGUID, LPCDIPROPHEADER)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::Acquire()
{
	void AcquireMouseInput(); AcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::Unacquire()
{
	void UnAcquireMouseInput(); UnAcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetDeviceState(DWORD, LPVOID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD)
{
	if(!IsMouseAcquired)
	{
		*pdwInOut = 0;
		return DI_OK;
	}

	if(!g_lastMouseEvents.empty())
	{
		do
		{
			RAWMOUSE& rawMouse = g_lastMouseEvents.front();
			if(rawMouse.lLastX != 0)
			{
				DIDEVICEOBJECTDATA xMotion;
				xMotion.dwOfs = DIMOFS_X;
				xMotion.dwData = static_cast<DWORD>(rawMouse.lLastX);
				g_generatedMouseEvents.push_back(xMotion);
			}
			if(rawMouse.lLastY != 0)
			{
				DIDEVICEOBJECTDATA yMotion;
				yMotion.dwOfs = DIMOFS_Y;
				yMotion.dwData = static_cast<DWORD>(rawMouse.lLastY);
				g_generatedMouseEvents.push_back(yMotion);
			}
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
			g_lastMouseEvents.pop_front();
		} while(!g_lastMouseEvents.empty());
	}

	extern HWND g_gothicHWND;
	if(g_generatedMouseEvents.empty() && GetForegroundWindow() == g_gothicHWND)
	{
		// We need to do it this way because Union Mouse wrapper
		// eats all mouse button events
		bool mouseButton0 = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		bool mouseButton1 = (GetAsyncKeyState(VK_RBUTTON) & 0x8000);
		bool mouseButton2 = (GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		bool mouseButton3 = (GetAsyncKeyState(VK_XBUTTON1) & 0x8000);
		bool mouseButton4 = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000);
		if(m_buttonState[0] != mouseButton0)
		{
			DIDEVICEOBJECTDATA mouseButton;
			mouseButton.dwOfs = DIMOFS_BUTTON0;
			mouseButton.dwData = (mouseButton0 ? 0x80 : 0);
			g_generatedMouseEvents.push_back(mouseButton);
			m_buttonState[0] = mouseButton0;
		}
		if(m_buttonState[1] != mouseButton1)
		{
			DIDEVICEOBJECTDATA mouseButton;
			mouseButton.dwOfs = DIMOFS_BUTTON1;
			mouseButton.dwData = (mouseButton1 ? 0x80 : 0);
			g_generatedMouseEvents.push_back(mouseButton);
			m_buttonState[1] = mouseButton1;
		}
		if(m_buttonState[2] != mouseButton2)
		{
			DIDEVICEOBJECTDATA mouseButton;
			mouseButton.dwOfs = DIMOFS_BUTTON2;
			mouseButton.dwData = (mouseButton2 ? 0x80 : 0);
			g_generatedMouseEvents.push_back(mouseButton);
			m_buttonState[2] = mouseButton2;
		}
		if(m_buttonState[3] != mouseButton3)
		{
			DIDEVICEOBJECTDATA mouseButton;
			mouseButton.dwOfs = DIMOFS_BUTTON3;
			mouseButton.dwData = (mouseButton3 ? 0x80 : 0);
			g_generatedMouseEvents.push_back(mouseButton);
			m_buttonState[3] = mouseButton3;
		}
		if(m_buttonState[4] != mouseButton4)
		{
			DIDEVICEOBJECTDATA mouseButton;
			mouseButton.dwOfs = DIMOFS_BUTTON4;
			mouseButton.dwData = (mouseButton4 ? 0x80 : 0);
			g_generatedMouseEvents.push_back(mouseButton);
			m_buttonState[4] = mouseButton4;
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

HRESULT m_IDirectInputMouseUnion7A::SetDataFormat(LPCDIDATAFORMAT)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::SetEventNotification(HANDLE)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	void InitMouseRawInput(HWND hwnd); InitMouseRawInput(hwnd);
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetDeviceInfo(LPDIDEVICEINSTANCEA)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::RunControlPanel(HWND, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::Initialize(HINSTANCE, DWORD, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT*, LPUNKNOWN)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetEffectInfo(LPDIEFFECTINFOA, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::GetForceFeedbackState(LPDWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::SendForceFeedbackCommand(DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::Escape(LPDIEFFESCAPE)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::Poll()
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::EnumEffectsInFile(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputMouseUnion7A::WriteEffectToFile(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD)
{
	// Not used
	return DI_OK;
}
