#include <Windows.h>
#include "hooks.hpp"

auto WINAPI ThreadProc(const PVOID hinstance) -> DWORD
{
    hooks::setup();

    while (true)
    {
        if (HIBYTE(GetAsyncKeyState(VK_DELETE)))
        {
            Beep(750, 100);
            break;
        }
        Sleep(50);
    }

    hooks::unhook();
    FreeLibraryAndExitThread((HINSTANCE)hinstance, 0);
}

auto WINAPI DllMain(const HINSTANCE hinstance, const DWORD reason, PVOID) -> BOOL
{
    if(reason == DLL_PROCESS_ATTACH)
    {
        const auto thread = CreateThread(nullptr, 0, ThreadProc, hinstance, 0, nullptr);
        CloseHandle(thread);
    }

    return TRUE;
}