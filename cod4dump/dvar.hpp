#pragma once
#include <cstdint>
#include "offsets.hpp"

namespace structs {
    using print_dvar_fn = char*(__cdecl*)(uint8_t type, char* buffer, uint32_t*, float, float);

    enum dvar_flag : uint16_t
    {
        write_protected = 0x10,
        read_only = 0x40,
        cheat_protected = 0x80,
    };

    enum dvar_type : uint8_t
    {
        dvar_boolean = 0,
        dvar_float,
        dvar_2d_vector,
        dvar_3d_vector,
        dvar_4d_vector,
        dvar_int,
        dvar_select,
        dvar_text,
        dvar_color,
    };

    constexpr const char* dvar_types[] = { "boolean", "float", "2d vector", "3d vector", "4d vector", "integer", "select", "text", "color" };

#pragma pack(push, 1)
    union dvar_value
    {
        uint8_t enabled;
        int32_t integer;
        uint32_t uint;
        float flt;
        float vector[4];
        const char* string;
        uint8_t color[4];
    };

    union dvar_limits
    {
        struct
        {
            int string_count;
            char** strings;
        } enumeration;

        struct
        {
            int min;
            int max;
        } integer;

        struct
        {
            float min;
            float max;
        } decimal;
    };

    struct dvar_s
    {
        const char* name;           // 0x0
        const char* description;    // 0x4
        uint16_t flags;             // 0x8
        uint8_t type;               // 0xA
        uint8_t modified;           // 0xB
        dvar_value current;
        dvar_value latched;
        dvar_value default_value;
        dvar_limits limits;
        dvar_s* prev;                 // 0x44
        dvar_s* next;                 // 0x48

        auto toggle_flag(const dvar_flag flag) -> bool {
            if (this->flags & flag)
                this->flags &= ~flag;
            else
                this->flags |= flag;
            return this->flags & flag;
        }
    };
#pragma pack(pop)

    // make sure the compiler isn't 200iq
    static_assert(sizeof(dvar_s) == 0x4C);

    inline auto get_dvars() -> dvar_s** {
        return reinterpret_cast<dvar_s**>(cvar_array);
    }

    inline auto get_dvar_count() -> uint32_t& {
        return *(uint32_t*)dvar_count;
    }

    auto get_cvar(const char* name)->dvar_s*;

}