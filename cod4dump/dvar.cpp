#include "dvar.hpp"
__declspec(noinline) auto get_cvar(const char* name) -> dvar_s* {
    dvar_s* result = nullptr;
    __asm {
        mov edi, name
        call find_dvar
        mov result, eax
    }
    return result;
}