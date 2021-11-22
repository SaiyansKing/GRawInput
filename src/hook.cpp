#include "hook.h"

void HookJMP(DWORD dwAddress, DWORD dwFunction)
{
	DWORD dwOldProtect, dwNewProtect, dwNewCall;
	dwNewCall = dwFunction - dwAddress - 5;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 5, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<BYTE*>(dwAddress) = 0xE9;
	*reinterpret_cast<DWORD*>(dwAddress + 1) = dwNewCall;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 5, dwOldProtect, &dwNewProtect);
}

void HookCall(DWORD dwAddress, DWORD dwFunction)
{
	DWORD dwOldProtect, dwNewProtect, dwNewCall;
	dwNewCall = dwFunction - dwAddress - 5;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 5, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<BYTE*>(dwAddress) = 0xE8;
	*reinterpret_cast<DWORD*>(dwAddress + 1) = dwNewCall;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 5, dwOldProtect, &dwNewProtect);
}

void HookCallN(DWORD dwAddress, DWORD dwFunction)
{
	DWORD dwOldProtect, dwNewProtect, dwNewCall;
	dwNewCall = dwFunction - dwAddress - 5;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 6, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<BYTE*>(dwAddress) = 0xE8;
	*reinterpret_cast<DWORD*>(dwAddress + 1) = dwNewCall;
	*reinterpret_cast<BYTE*>(dwAddress + 5) = 0x90;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), 6, dwOldProtect, &dwNewProtect);
}

void WriteStack(DWORD dwAddress, BYTE* stack, DWORD len)
{
	DWORD dwOldProtect, dwNewProtect;
	VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), len, PAGE_READWRITE, &dwOldProtect);
	memcpy(reinterpret_cast<LPVOID>(dwAddress), stack, len);
	VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), len, dwOldProtect, &dwNewProtect);
}

void Nop(DWORD dwAddress, int size)
{
	DWORD dwOldProtect, dwNewProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), size, PAGE_READWRITE, &dwOldProtect);
    memset(reinterpret_cast<LPVOID>(dwAddress), 0x90, size);
    VirtualProtect(reinterpret_cast<LPVOID>(dwAddress), size, dwOldProtect, &dwNewProtect);
}

void OverWriteByte(DWORD addressToOverWrite, BYTE newValue)
{
    DWORD dwOldProtect, dwNewProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 1, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<BYTE*>(addressToOverWrite) = newValue;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 1, dwOldProtect, &dwNewProtect);
}

void OverWriteWord(DWORD addressToOverWrite, WORD newValue)
{
    DWORD dwOldProtect, dwNewProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 2, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<WORD*>(addressToOverWrite) = newValue;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 2, dwOldProtect, &dwNewProtect);
}

void OverWrite(DWORD addressToOverWrite, DWORD newValue)
{
    DWORD dwOldProtect, dwNewProtect;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 4, PAGE_READWRITE, &dwOldProtect);
	*reinterpret_cast<DWORD*>(addressToOverWrite) = newValue;
    VirtualProtect(reinterpret_cast<LPVOID>(addressToOverWrite), 4, dwOldProtect, &dwNewProtect);
}
