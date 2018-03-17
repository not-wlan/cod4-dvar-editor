#pragma once
#include <d3dx9.h>

namespace hooks {
    auto __stdcall hooked_EndScene(IDirect3DDevice9* device)->HRESULT;
    auto __stdcall hooked_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp)->HRESULT;
    auto __stdcall hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT;

    using EndSceneFn = decltype(&hooked_EndScene);
    using ResetFn = decltype(&hooked_Reset);

    namespace index {
        constexpr auto reset = 16;
        constexpr auto end_scene = 42;
    }

    void setup();
    void unhook();
}