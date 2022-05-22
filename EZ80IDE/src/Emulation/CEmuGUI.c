#pragma once

#include "core/cemu.h"
#if GBC_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

void gui_console_clear(void)
{
	// Uhhh...... shhh.
#if GBC_PLATFORM_WINDOWS
	system("cls");
#elif GBC_PLATFORM_LINUX
	system("clear; clear");
#endif
}

void gui_console_printf(const char* format, ...)
{
	// TODO: make this use spdlog
	va_list args;
	va_start(args, format);
	(void)vfprintf_s(stdout, format, args);
	(void)fflush(stdout);
	va_end(args);
}

void gui_console_err_printf(const char* format, ...)
{
	// TODO: make this use spdlog
	va_list args;
	va_start(args, format);
	(void)vfprintf_s(stderr, format, args);
	(void)fflush(stderr);
	va_end(args);
}

#ifdef DEBUG_SUPPORT
void gui_debug_open(int reason, uint32_t data)
{
	gui_console_printf("[CEmu debug open] (reason=%d, data=0x%X)\n", reason, data);
}

void gui_debug_close(void)
{
	gui_console_printf("[CEmu debug close]");
}
#endif
