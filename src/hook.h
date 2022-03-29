#pragma once
#include <windows.h>

void HookJMP(DWORD dwAddress, DWORD dwFunction);
void HookCall(DWORD dwAddress, DWORD dwFunction);
void HookCallN(DWORD dwAddress, DWORD dwFunction);
void WriteStack(DWORD dwAddress, const char* stack, DWORD len);
void Nop(DWORD dwAddress, int size);
void OverWriteByte(DWORD addressToOverWrite, BYTE newValue);
void OverWriteWord(DWORD addressToOverWrite, WORD newValue);
void OverWrite(DWORD addressToOverWrite, DWORD newValue);
void OverWriteFloat(DWORD addressToOverWrite, float newValue);

template<typename T, size_t len>
void WriteStack(DWORD dwAddress, T(&stack)[len])
{
	WriteStack(dwAddress, reinterpret_cast<const char*>(stack), len - 1);
}
