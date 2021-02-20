#include "cracking.hpp"
#include <windows.h>

void __jmp(int from, int to) {
    DWORD tmp;
    VirtualProtect((void*)from, 5, PAGE_EXECUTE_READWRITE, &tmp);
    int relative = to - (from + 5);
    memset((void*)from, 0xE9, 1);
    memcpy((void*)(from + 1), &relative, 4);
    VirtualProtect((void*)from, 5, tmp, &tmp);
}

void __call(int from, int to) {
    DWORD tmp;
    VirtualProtect((void*)from, 5, PAGE_EXECUTE_READWRITE, &tmp);
    int relative = to - (from + 5);
    memcpy((void*)(from + 1), &relative, 4);
    VirtualProtect((void*)from, 5, tmp, &tmp);
}