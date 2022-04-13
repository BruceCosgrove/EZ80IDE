#pragma once

#include "GBC/Rendering/Context.h"

struct GLFWwindow;

namespace gbc
{
	class OpenGLContext : public Context
	{
	public:
		OpenGLContext(void* nativeWindow);
	public:
		virtual void SwapBuffers() override;
		virtual Timestep GetElapsedTime() const override;
	public:
		virtual const char* GetVendor() const override;
		virtual const char* GetRenderer() const override;
		virtual const char* GetVersion() const override;
	public:
#if GBC_ENABLE_IMGUI
		virtual const char* GetVersionForImGui() const override;
#endif
	private:
		GLFWwindow* window = nullptr;
		mutable float lastTime = 0.0f;
	};
}
