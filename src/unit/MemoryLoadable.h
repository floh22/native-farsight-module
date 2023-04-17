#pragma once
#include "Windows.h"

class MemoryLoadable {
    virtual void LoadFromMemory(DWORD64 base, HANDLE hProcess, bool deepLoad = true) = 0;
};