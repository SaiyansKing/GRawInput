#include "IDirectInput7A.h"
#include "IDirectInputJoystick7A.h"

HRESULT m_IDirectInputJoystick7A::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	(void)riid;
	(void)ppvObj;

	return DIERR_GENERIC;
}

ULONG m_IDirectInputJoystick7A::AddRef()
{
	return ++m_references;
}

ULONG m_IDirectInputJoystick7A::Release()
{
	ULONG ref = --m_references;
	if(ref == 0) delete this;
	return ref;
}

HRESULT m_IDirectInputJoystick7A::GetCapabilities(LPDIDEVCAPS)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetProperty(REFGUID, LPDIPROPHEADER)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SetProperty(REFGUID, LPCDIPROPHEADER)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::Acquire()
{
	//void AcquireMouseInput(); AcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::Unacquire()
{
	//void UnAcquireMouseInput(); UnAcquireMouseInput();
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetDeviceState(DWORD, LPVOID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetDeviceData(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD pdwInOut, DWORD)
{
	// Used but not needed
	*pdwInOut = 0;
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SetDataFormat(LPCDIDATAFORMAT)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SetEventNotification(HANDLE)
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SetCooperativeLevel(HWND hwnd, DWORD dwFlags)
{
	//void InitMouseRawInput(HWND hwnd); InitMouseRawInput(hwnd);
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetDeviceInfo(LPDIDEVICEINSTANCEA)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::RunControlPanel(HWND, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::Initialize(HINSTANCE, DWORD, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::CreateEffect(REFGUID, LPCDIEFFECT, LPDIRECTINPUTEFFECT*, LPUNKNOWN)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::EnumEffects(LPDIENUMEFFECTSCALLBACKA, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetEffectInfo(LPDIEFFECTINFOA, REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::GetForceFeedbackState(LPDWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SendForceFeedbackCommand(DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::Escape(LPDIEFFESCAPE)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::Poll()
{
	// Used but not needed
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::SendDeviceData(DWORD, LPCDIDEVICEOBJECTDATA, LPDWORD, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::EnumEffectsInFile(LPCSTR, LPDIENUMEFFECTSINFILECALLBACK, LPVOID, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInputJoystick7A::WriteEffectToFile(LPCSTR, DWORD, LPDIFILEEFFECT, DWORD)
{
	// Not used
	return DI_OK;
}
