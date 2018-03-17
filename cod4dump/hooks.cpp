#include "hooks.hpp"

#include <tchar.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"

#include "vfunc_hook.hpp"
#include "offsets.hpp"
#include "menu.hpp"

namespace hooks
{
    vfunc_hook dx9_hook;
    HWND window = nullptr;
    bool pressed_keys[256] = {false};
    WNDPROC old_wndproc = nullptr;

    auto setup() -> void
    {
        dx9_hook.setup(*reinterpret_cast<IDirect3DDevice9**>(dx9_device));
        dx9_hook.hook_index(index::end_scene, hooked_EndScene);
        dx9_hook.hook_index(index::reset, hooked_Reset);

        window = FindWindow(_T("CoD4"), nullptr);
        assert(window != nullptr);

        old_wndproc = WNDPROC(SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(hooked_WndProc)));
    }

    auto unhook() -> void
    {
        WNDPROC(SetWindowLongPtr(window, GWLP_WNDPROC, LONG_PTR(old_wndproc)));
        dx9_hook.unhook_all();
        menu::fix_mouse();
    }


    auto __stdcall hooked_EndScene(IDirect3DDevice9* device) -> HRESULT
    {
        if (!menu::is_initialized())
            menu::initialize(device, window);

        ImGui_ImplDX9_NewFrame();

        if (menu::is_open())
            menu::render_menu();

        ImGui::EndFrame();
        ImGui::Render();

        return dx9_hook.get_original<EndSceneFn>(42)(device);
    }

    auto __stdcall hooked_Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp) -> HRESULT
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        const auto result = dx9_hook.get_original<ResetFn>(16)(device, pp);
        ImGui_ImplDX9_CreateDeviceObjects();
        return result;
    }

    auto __stdcall hooked_WndProc(const HWND h_wnd, const UINT u_msg, const WPARAM w_param, const LPARAM l_param) -> LRESULT
    {
        switch (u_msg) {
        case WM_LBUTTONDOWN:
            pressed_keys[VK_LBUTTON] = true;
            break;
        case WM_LBUTTONUP:
            pressed_keys[VK_LBUTTON] = false;
            break;
        case WM_RBUTTONDOWN:
            pressed_keys[VK_RBUTTON] = true;
            break;
        case WM_RBUTTONUP:
            pressed_keys[VK_RBUTTON] = false;
            break;
        case WM_KEYDOWN:
            pressed_keys[w_param] = true;
            break;
        case WM_KEYUP:
            pressed_keys[w_param] = false;
            break;
        default: break;
        }
        {
            static auto is_down = false;
            static auto is_clicked = false;
            if (pressed_keys[VK_INSERT]) {
                is_clicked = false;
                is_down = true;
            }
            else if (!pressed_keys[VK_INSERT] && is_down) {
                is_clicked = true;
                is_down = false;
            }
            else {
                is_clicked = false;
                is_down = false;
            }

            if (is_clicked)
                menu::toggle();

        }

        if (menu::is_initialized() && menu::is_open() && ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param))
            return true;

        return CallWindowProc(old_wndproc, h_wnd, u_msg, w_param, l_param);
    }
}
