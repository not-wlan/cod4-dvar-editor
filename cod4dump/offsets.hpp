#pragma once
#include <cstdint>
// xref return value of Direct3DCreate9 and set types and you'll find the CreateDevice call
constexpr uintptr_t dx9_device = 0x0CC9A408;
// search for "%s is cheat protected.\n" it's the global variable that is being checked.
constexpr uintptr_t cvar_array = 0x0CBA7404;
// find the function that adds dvars and it's the first call in it to check the existance
constexpr uintptr_t find_dvar = 0x0056B5D0;
// xref "Can't create dvar '%s': %i dvars already exist", it's the value compared to
constexpr uintptr_t dvar_count = 0x0CBA73F8;
// check callback of in_restart
constexpr uintptr_t restart_input = 0x00575E90;
// check callback of in_restart
constexpr uintptr_t mouse_enabled = 0x0CC147D1;
// search for "Domain is 0 or 1"
constexpr uintptr_t format_domains = 0x0056AC00;