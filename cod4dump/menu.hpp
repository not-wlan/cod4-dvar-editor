#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"

namespace menu {
    auto initialize(IDirect3DDevice9* device, const HWND window) -> void;
    auto is_initialized() -> bool;
    auto render_menu() -> void;
    auto is_open() -> bool;
    auto toggle() -> void;
    auto fix_mouse() -> void;
}