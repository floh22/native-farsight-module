#pragma once
#include "Windows.h"

class MemoryLoadable {
    virtual void LoadFromMemory(DWORD base, HANDLE hProcess, bool deepLoad = true) = 0;
};