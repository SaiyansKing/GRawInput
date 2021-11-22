#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

class m_IDirectInput7A : public IDirectInput7A
{
	public:
		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj);
		STDMETHOD_(ULONG, AddRef)(THIS);
		STDMETHOD_(ULONG, Release)(THIS);

		/*** IDirectInputA methods ***/
		STDMETHOD(CreateDevice)(THIS_ REFGUID, LPDIRECTINPUTDEVICEA*, LPUNKNOWN);
		STDMETHOD(EnumDevices)(THIS_ DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD);
		STDMETHOD(GetDeviceStatus)(THIS_ REFGUID);
		STDMETHOD(RunControlPanel)(THIS_ HWND, DWORD);
		STDMETHOD(Initialize)(THIS_ HINSTANCE, DWORD);
		/*** IDirectInput2A methods ***/
		STDMETHOD(FindDevice)(THIS_ REFGUID, LPCSTR, LPGUID);
		/*** IDirectInput7A methods ***/
		STDMETHOD(CreateDeviceEx)(THIS_ REFGUID, REFIID, LPVOID*, LPUNKNOWN);

	private:
		ULONG m_references = 1;
};
