#pragma once

#if GBC_CONFIG_DEBUG
	#if GBC_PLATFORM_WINDOWS
		#define GBC_DEBUGBREAK() __debugbreak()
	#else
		#error Platform doesn't support debugbreak!
	#endif
#else
	#define GBC_DEBUGBREAK()
#endif
