#include "gbcpch.h"
#include "OpenGLContext.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>

namespace gbc
{
#if GBC_ENABLE_IMGUI
	static char version[] = "#version XX0 core";
#endif

	OpenGLContext::OpenGLContext(void* nativeWindow)
	{
		GBC_PROFILE_FUNCTION();

		window = static_cast<GLFWwindow*>(nativeWindow);

		GBC_CORE_ASSERT(window != nullptr, "Window is nullptr!");
		glfwMakeContextCurrent(window);
		int initStatus = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GBC_CORE_ASSERT(initStatus != 0, "Failed to initialize glad!");

#if GBC_ENABLE_IMGUI
		version[9] = static_cast<char>(GLVersion.major + '0');
		version[10] = static_cast<char>(GLVersion.minor + '0');
#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		GBC_PROFILE_FUNCTION();

		glfwSwapBuffers(window);
	}

	Timestep OpenGLContext::GetElapsedTime() const
	{
		float time = static_cast<float>(glfwGetTime());
		float elapsedTime = time - lastTime;
		lastTime = time;
		return elapsedTime;
	}

	const char* OpenGLContext::GetVendor() const
	{
		return (const char*)glGetString(GL_VENDOR);
	}

	const char* OpenGLContext::GetRenderer() const
	{
		return (const char*)glGetString(GL_RENDERER);
	}

	const char* OpenGLContext::GetVersion() const
	{
		return (const char*)glGetString(GL_VERSION);
	}

#if GBC_ENABLE_IMGUI
	const char* OpenGLContext::GetVersionForImGui() const { return version; }
#endif
}
