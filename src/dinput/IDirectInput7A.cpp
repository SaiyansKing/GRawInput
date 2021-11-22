#include "IDirectInput7A.h"
#include "IDirectInputMouse7A.h"
#include "IDirectInputMouseUnion7A.h"
#include "IDirectInputKeyboard7A.h"
#include "IDirectInputJoystick7A.h"

#define DIDEVTYPE_DEVICE 1
#define DIDEVTYPE_MOUSE 2
#define DIDEVTYPE_KEYBOARD 3
#define DIDEVTYPE_JOYSTICK 4

extern bool IsEnabledUnionWrapper;

HRESULT m_IDirectInput7A::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	(void)riid;
	(void)ppvObj;

	return DIERR_GENERIC;
}

ULONG m_IDirectInput7A::AddRef()
{
	return ++m_references;
}

ULONG m_IDirectInput7A::Release()
{
	ULONG ref = --m_references;
	if(ref == 0) delete this;
	return ref;
}

HRESULT m_IDirectInput7A::CreateDevice(REFGUID, LPDIRECTINPUTDEVICEA*, LPUNKNOWN)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInput7A::EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags)
{
	if(dwDevType != DIDEVTYPE_JOYSTICK)
		return DIERR_INVALIDPARAM;

	DIDEVICEINSTANCEA joystickDevice;
	joystickDevice.dwSize = sizeof(DIDEVICEINSTANCEA);
	joystickDevice.guidInstance = { 0xF5049E78, 0x4861, 0x11D2, {0xA4, 0x07, 0x00, 0xA0, 0xC9, 0x06, 0x29, 0xA8} };
	joystickDevice.guidProduct = { 0xF5049E78, 0x4861, 0x11D2, {0xA4, 0x07, 0x00, 0xA0, 0xC9, 0x06, 0x29, 0xA8} };
	joystickDevice.dwDevType = DIDEVTYPE_JOYSTICK;
	sprintf_s(joystickDevice.tszInstanceName, "Gamepad");
	sprintf_s(joystickDevice.tszProductName, "Gamepad");
	joystickDevice.guidFFDriver = { 0xF5049E78, 0x4861, 0x11D2, {0xA4, 0x07, 0x00, 0xA0, 0xC9, 0x06, 0x29, 0xA8} };
	joystickDevice.wUsagePage = 0;
	joystickDevice.wUsage = 0;
	lpCallback(&joystickDevice, pvRef);
	return DI_OK;
}

HRESULT m_IDirectInput7A::GetDeviceStatus(REFGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInput7A::RunControlPanel(HWND, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInput7A::Initialize(HINSTANCE, DWORD)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInput7A::FindDevice(REFGUID, LPCSTR, LPGUID)
{
	// Not used
	return DI_OK;
}

HRESULT m_IDirectInput7A::CreateDeviceEx(REFGUID rguid, REFIID, LPVOID* lplpDirectInputDevice, LPUNKNOWN)
{
	void CheckUnionWrapper(); CheckUnionWrapper();

	const GUID JoyStick = { 0xF5049E78, 0x4861, 0x11D2, { 0xA4, 0x07, 0x00, 0xA0, 0xC9, 0x06, 0x29, 0xA8 } };
	if(rguid == GUID_SysKeyboard) *lplpDirectInputDevice = new m_IDirectInputKeyboard7A();
	else if(rguid == GUID_SysMouse) *lplpDirectInputDevice = (IsEnabledUnionWrapper ? reinterpret_cast<void*>(new m_IDirectInputMouseUnion7A()) : reinterpret_cast<void*>(new m_IDirectInputMouse7A()));
	else if(rguid == JoyStick) *lplpDirectInputDevice = new m_IDirectInputJoystick7A();
	else return DIERR_GENERIC;
	return DI_OK;
}
