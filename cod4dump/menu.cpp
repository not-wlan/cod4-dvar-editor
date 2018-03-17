#include "menu.hpp"
#include "dvar.hpp"

namespace menu {
    bool initialized = false;
    char search_buffer[64] = { 0 };
    bool menu_open = false;

    constexpr auto button_title = "edit";
    constexpr auto popup_title = "editpopup";
    constexpr auto window_title = "cod4 dvar editor";

    auto is_initialized() -> bool
    {
        return initialized;
    }

    auto render_cvar(dvar_s* dvar) -> void {
        const auto& type = dvar->type;

        ImGui::Text("name: %s", dvar->name);
        ImGui::Text("description: %s", dvar->description);
        ImGui::Text("type: %s", dvar_types[dvar->type]);
        ImGui::Text("pointer: 0x%p", dvar);

        ImGui::Separator();

        char buffer[1024];
        ImGui::Text(((print_dvar_fn)format_domains)(dvar->type, buffer, nullptr, dvar->limits.decimal.min, dvar->limits.decimal.max));

        ImGui::Separator();

        ImGui::Columns(3);
        ImGui::Text("modified");
        ImGui::NextColumn();
        ImGui::Text((dvar->modified == 0) ? "false" : "true");
        ImGui::NextColumn();
        if (ImGui::Button("toggle##modified"))
            dvar->modified = (dvar->modified == 0) ? 1 : 0;

        ImGui::Columns(1);

        
        ImGui::Separator();

        ImGui::Text("flags:");

        ImGui::Separator();

        ImGui::Columns(3);
         
        ImGui::Text("cheat");
        ImGui::NextColumn();
        ImGui::Text((dvar->flags & cheat_protected) ? "true" : "false");
        ImGui::NextColumn();
        if(ImGui::Button("toggle##cheat"))
            dvar->toggle_flag(cheat_protected);
        ImGui::NextColumn();

        ImGui::Text("read only");
        ImGui::NextColumn();
        ImGui::Text((dvar->flags & read_only) ? "true" : "false");
        ImGui::NextColumn();
        if (ImGui::Button("toggle##read_only"))
            dvar->toggle_flag(read_only);
        ImGui::NextColumn();

        ImGui::Text("protected");
        ImGui::NextColumn();
        ImGui::Text((dvar->flags & write_protected) ? "true" : "false");
        ImGui::NextColumn();
        if (ImGui::Button("toggle##write_protected"))
            dvar->toggle_flag(write_protected);
        ImGui::NextColumn();

        ImGui::Columns(1);

        ImGui::Separator();

        switch(type){
        case dvar_boolean:
            {
            ImGui::Text("values:");

            ImGui::Separator();
            ImGui::Columns(3);
            ImGui::Text("current");
            ImGui::NextColumn();
            ImGui::Text(dvar->current.enabled == 0 ? "false" : "true");
            ImGui::NextColumn();
            if (ImGui::Button("toggle##current"))
                dvar->current.enabled = (dvar->current.enabled == 0) ? 1 : 0;
            ImGui::NextColumn();

            ImGui::Text("latched");
            ImGui::NextColumn();
            ImGui::Text(dvar->latched.enabled == 0 ? "false" : "true");
            ImGui::NextColumn();
            if (ImGui::Button("toggle##latched"))
                dvar->latched.enabled = (dvar->latched.enabled == 0) ? 1 : 0;
            ImGui::NextColumn();

            ImGui::Text("default");
            ImGui::NextColumn();
            ImGui::Text(dvar->default_value.enabled == 0 ? "false" : "true");
            ImGui::NextColumn();
            if (ImGui::Button("toggle##default_value"))
                dvar->default_value.enabled = (dvar->default_value.enabled == 0) ? 1 : 0;
            ImGui::NextColumn();
            ImGui::Columns(1);
            break;
            }
        case dvar_text:
            {
            ImGui::Text("values:");
            ImGui::Separator();

            ImGui::Text("current: %s", dvar->current.string);
            ImGui::Text("latched: %s", dvar->latched.string);
            ImGui::Text("default: %s", dvar->default_value.string);
            break;
            }
        case dvar_float:
            {
            ImGui::SliderFloat("value", &dvar->current.flt, dvar->limits.decimal.min, dvar->limits.decimal.max);
            if (ImGui::Button("reset##float"))
                dvar->current.flt = dvar->default_value.flt;
            ImGui::SameLine();
            if (ImGui::Button("unlatch##float"))
                dvar->latched.flt = dvar->current.flt;
            break;
            }
        case dvar_int:
        {
            ImGui::SliderInt("value", &dvar->current.integer, dvar->limits.integer.min, dvar->limits.integer.max);
            if (ImGui::Button("reset##int"))
                dvar->current.integer = dvar->default_value.integer;
            ImGui::SameLine();
            if (ImGui::Button("unlatch##int"))
                dvar->latched.integer = dvar->current.integer;
            break;
        }
        case dvar_color:
            {
            float colorf[4] = { dvar->current.color[0] / 255.f, dvar->current.color[1] / 255.f , dvar->current.color[2] / 255.f ,dvar->current.color[3] / 255.f };
            ImGui::ColorPicker4("current", colorf, ImGuiColorEditFlags_NoPicker);
            break;
            }
        default: break;
        }

        ImGui::Separator();

        if (ImGui::Button("close"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    auto render_menu() -> void
    {
        ImGui::SetNextWindowSize({ 450, 450 });
        ImGui::SetNextWindowPosCenter();
        ImGui::Begin(window_title, nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::InputText("search", search_buffer, sizeof search_buffer);

        const auto dvars = get_dvars();
        const auto do_search = strlen(search_buffer) != 0;

        ImGui::Columns(2);
        
        ImGui::SetColumnOffset(1, 375);
        
        ImGui::Text("name");
        ImGui::NextColumn();
        // empty
        ImGui::NextColumn();

        // start at one because sv_cheats is twice in the array
        for (uint32_t i = 1; i < get_dvar_count(); ++i)
        {
            const auto dvar = dvars[i];

            if (do_search && strstr(dvar->name, search_buffer) == nullptr)
                continue;
            
            ImGui::PushID(dvar->name);
            if(dvar->type == dvar_select)
                ImGui::Text("+ %s", dvar->name);
            else
                ImGui::Text(dvar->name);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(dvar->description);

            ImGui::NextColumn();

            if(ImGui::Button(button_title))
            {
                ImGui::OpenPopup(popup_title);
            }

            if(ImGui::BeginPopupModal(popup_title, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                render_cvar(dvar);

            ImGui::NextColumn();

            ImGui::PopID();
        }
        ImGui::Columns(1);

        ImGui::End();
    }

    auto is_open() -> bool
    {
        return menu_open;
    }

    auto toggle() -> void
    {
        menu_open = !menu_open;
        static auto in_mouse = get_cvar("in_mouse");

        if (in_mouse != nullptr)
        {
            in_mouse->current.enabled = menu_open ? 0 : 1;
            in_mouse->latched.enabled = menu_open ? 0 : 1;
            in_mouse->modified = 0;

            using restart_input_fn = void (*)();
            ((restart_input_fn)restart_input)();
            auto& mouse = *(uint8_t*)mouse_enabled;
            mouse = in_mouse->current.enabled;            
        }
    }

    auto initialize(IDirect3DDevice9* device, const HWND window) -> void {
        ImGui::CreateContext();
        ImGui_ImplDX9_Init(window, device);
        initialized = true;
    }
}
