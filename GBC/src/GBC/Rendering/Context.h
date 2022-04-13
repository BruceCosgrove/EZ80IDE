#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Core/Timestep.h"

namespace gbc
{
	class Context
	{
	public:
		virtual ~Context() = default;

		virtual void SwapBuffers() = 0;
		virtual Timestep GetElapsedTime() const = 0;

		virtual const char* GetVendor() const = 0;
		virtual const char* GetRenderer() const = 0;
		virtual const char* GetVersion() const = 0;

#if GBC_ENABLE_IMGUI
		virtual const char* GetVersionForImGui() const = 0;
#endif

		static Scope<Context> Create(void* nativeWindow);
	};
}
