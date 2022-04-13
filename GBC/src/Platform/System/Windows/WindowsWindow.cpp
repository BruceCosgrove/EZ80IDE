#include "gbcpch.h"
#include "WindowsWindow.h"
#include "GBC/Core/Application.h"
#include "GBC/Core/Input.h"
#include "GBC/Events/KeyEvents.h"
#include "GBC/Events/MouseEvents.h"
#include "GBC/Events/WindowEvents.h"
#if GBC_CONFIG_DEBUG
	#include "GBC/Rendering/RendererAPI.h"
#endif
#include <glfw/glfw3.h>

namespace gbc
{
	static constexpr Mods glfwModsToGBCMods(int32_t mods)
	{
		return (mods & GLFW_MOD_SHIFT     ? Mods_Shift    : Mods_None) |
			   (mods & GLFW_MOD_CONTROL   ? Mods_Control  : Mods_None) |
			   (mods & GLFW_MOD_ALT       ? Mods_Alt      : Mods_None) |
			   (mods & GLFW_MOD_SUPER     ? Mods_Super    : Mods_None) |
			   (mods & GLFW_MOD_CAPS_LOCK ? Mods_CapsLock : Mods_None) |
			   (mods & GLFW_MOD_NUM_LOCK  ? Mods_NumLock  : Mods_None);
	}

	static uint8_t glfwWindowCount = 0;
	static void glfwErrorCallback(int32_t error, const char* description)
	{
		GBC_CORE_ERROR("GLFW Error (id={0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowSpecifications& specification)
	{
		return CreateScope<WindowsWindow>(specification);
	}

	WindowsWindow::WindowsWindow(const WindowSpecifications& specification)
	{
		GBC_PROFILE_FUNCTION();

		if (glfwWindowCount == 0)
		{
			GBC_PROFILE_SCOPE("glfwInit");

			int32_t initState = glfwInit();
			GBC_CORE_ASSERT(initState == GLFW_TRUE, "Failed to initialize GLFW!");
			glfwSetErrorCallback(glfwErrorCallback);
		}

		state.current.size = { specification.width, specification.height };
		state.title = specification.title;
		state.resizable = specification.resizable;
		state.fullscreen = specification.fullscreen;
		state.focused = specification.focusOnShow;

		glfwWindowHint(GLFW_RESIZABLE, state.resizable);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, state.focused);
#if GBC_CONFIG_DEBUG
		if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

		if (state.fullscreen)
		{
			SaveDimensions();
			state.current.position = { 0, 0 };

			const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
			state.current.size = { videoMode->width, videoMode->height };
		}

		{
			GBC_PROFILE_SCOPE("glfwCreateWindow");
			window = glfwCreateWindow(state.current.size.x, state.current.size.y, state.title.c_str(), state.fullscreen ? primaryMonitor : nullptr, nullptr);
		}
		GBC_CORE_ASSERT(window != nullptr, "Failed to create window!");
		glfwWindowCount++;

		if (!state.fullscreen)
		{
			int left, top, right, bottom;
			glfwGetMonitorWorkarea(primaryMonitor, &left, &top, &right, &bottom);
			glm::ivec2 monitorWorkArea(right - left, bottom - top);
			glfwGetWindowFrameSize(window, &left, &top, &right, &bottom);
			glm::ivec2 windowFrameSize(right + left, bottom + top);

			// Calculate the offset needed for the window to be centered in the primary monitor
			state.current.position = glm::ivec2(left, top) + (monitorWorkArea - (state.current.size + windowFrameSize)) / 2;
		}

		glfwSetWindowPos(window, state.current.position.x, state.current.position.y);
		glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
		glfwSetWindowUserPointer(window, &state);

		context = Context::Create(window);
		SetVSync(specification.vsync);
		SetCaptureMouse(specification.captureMouse);

		if (specification.maximizeOnShow)
		{
			glfwMaximizeWindow(window);
			glfwGetWindowSize(window, &state.current.size.x, &state.current.size.y);
			glfwGetWindowPos(window, &state.current.position.x, &state.current.position.y);
			state.maximized = true;
		}

		SetCallbacks(window);
		glfwShowWindow(window);
	}

	WindowsWindow::~WindowsWindow()
	{
		GBC_PROFILE_FUNCTION();

		glfwDestroyWindow(window);
		if (--glfwWindowCount == 0)
			glfwTerminate();
	}

	void WindowsWindow::PollEvents()
	{
		GBC_PROFILE_FUNCTION();

		// Only call on thread that called glfwInit and once per all windows
		glfwPollEvents();
	}

	void WindowsWindow::SwapBuffers()
	{
		context->SwapBuffers();
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		glfwSetWindowTitle(window, title.c_str());
		state.title = title;
	}

	void WindowsWindow::SetIcon(const Ref<LocalTexture2D>& texture)
	{
		if (texture)
		{
			GLFWimage image{ texture->GetWidth(), texture->GetHeight(), texture->GetData() };
			glfwSetWindowIcon(window, 1, &image);
		}
		else
			glfwSetWindowIcon(window, 0, nullptr);
	}

	void WindowsWindow::SetVSync(bool vsync)
	{
		glfwSwapInterval(vsync);
		state.vsync = vsync;
	}

	void WindowsWindow::SetResizable(bool resizable)
	{
		glfwSetWindowAttrib(window, GLFW_RESIZABLE, resizable);
		state.resizable = resizable;
	}

	void WindowsWindow::SetFullscreen(bool fullscreen)
	{
		if (fullscreen)
		{
			SaveDimensions();

			// From: https://stackoverflow.com/questions/21421074/how-to-create-a-full-screen-window-on-the-current-monitor-with-glfw
			// Get the monitor that most of the window is on
			int32_t largestOverlap = INT_MIN;
			GLFWmonitor* monitor = nullptr;

			int32_t monitorCount;
			GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

			for (int32_t i = 0; i < monitorCount; i++)
			{
				const GLFWvidmode* videoMode = glfwGetVideoMode(monitors[i]);

				int32_t monitorX, monitorY;
				glfwGetMonitorPos(monitors[i], &monitorX, &monitorY);

				int32_t overlapX = std::max(0, std::min(state.current.position.x + state.current.size.x, monitorX + videoMode->width) - std::max(state.current.position.x, monitorX));
				int32_t overlapY = std::max(0, std::min(state.current.position.y + state.current.size.y, monitorY + videoMode->height) - std::max(state.current.position.y, monitorY));
				int32_t overlap = overlapX * overlapY;

				if (overlap > largestOverlap)
				{
					largestOverlap = overlap;
					monitor = monitors[i];
				}
			}

			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
		}
		else
			glfwSetWindowMonitor(window, nullptr, state.preFullscreen.position.x, state.preFullscreen.position.y, state.preFullscreen.size.x, state.preFullscreen.size.y, GLFW_DONT_CARE);

		SetVSync(IsVSync());

		state.fullscreen = fullscreen;
	}

	void WindowsWindow::SetCaptureMouse(bool captureMouse)
	{
		glfwSetInputMode(window, GLFW_CURSOR, captureMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		state.captureMouse = captureMouse;
	}

	void WindowsWindow::SaveDimensions()
	{
		state.preFullscreen.position.x = state.current.position.x;
		state.preFullscreen.position.y = state.current.position.y;
		state.preFullscreen.size.x = state.current.size.x;
		state.preFullscreen.size.y = state.current.size.y;
	}

	// Callbacks

	void WindowsWindow::SetCallbacks(GLFWwindow* window)
	{
		// Window Events
		glfwSetWindowCloseCallback(window, WindowCloseCallback);
		glfwSetWindowSizeCallback(window, WindowSizeCallback);
		glfwSetWindowPosCallback(window, WindowPosCallback);
		glfwSetWindowFocusCallback(window, WindowFocusCallback);
		glfwSetWindowIconifyCallback(window, WindowIconifyCallback);
		glfwSetWindowMaximizeCallback(window, WindowMaximizeCallback);
		glfwSetDropCallback(window, DropCallback);
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
		glfwSetWindowContentScaleCallback(window, WindowContentScaleCallback);
		glfwSetWindowRefreshCallback(window, WindowRefreshCallback);

		// Key Events
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetCharCallback(window, CharCallback);
		glfwSetCharModsCallback(window, CharModsCallback);

		// Mouse Events
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwSetCursorPosCallback(window, CursorPosCallback);
		glfwSetScrollCallback(window, ScrollCallback);
		glfwSetCursorEnterCallback(window, CursorEnterCallback);
	}

#define BEGIN_EVENT_CALLBACK GLFWwindow* context = glfwGetCurrentContext(); glfwMakeContextCurrent(window)
#define END_EVENT_CALLBACK glfwMakeContextCurrent(context)

	void WindowsWindow::WindowCloseCallback(GLFWwindow* window)
	{
		BEGIN_EVENT_CALLBACK;

		// Since ImGui's glfw implementation doesn't set a window user pointer, this is fine
		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			// Only send a close event if the window is not owned by ImGui
			WindowCloseEvent event(window);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowResizeEvent event(window, width, height);
			state->current.size = { width, height };
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowPosCallback(GLFWwindow* window, int32_t x, int32_t y)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowMoveEvent event(window, x, y);
			state->current.position = { x, y };
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowFocusCallback(GLFWwindow* window, int32_t focused)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowFocusEvent event(window, focused == GLFW_TRUE);
			state->focused = event.IsFocused();
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowIconifyCallback(GLFWwindow* window, int32_t iconified)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowMinimizeEvent event(window, iconified == GLFW_TRUE);
			state->minimized = event.IsMinimized();
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowMaximizeCallback(GLFWwindow* window, int32_t maximized)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowMaximizeEvent event(window, maximized == GLFW_TRUE);
			state->maximized = event.IsMaximized();
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::DropCallback(GLFWwindow* window, int32_t pathCount, const char** paths)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowDropEvent event(window, pathCount, paths);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::FramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowFramebufferResizeEvent event(window, width, height);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowContentScaleEvent event(window, scaleX, scaleY);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::WindowRefreshCallback(GLFWwindow* window)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			WindowRefreshEvent event(window);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}


	void WindowsWindow::KeyCallback(GLFWwindow* window, int32_t keycode, int32_t scancode, int32_t action, int32_t mods)
	{
		BEGIN_EVENT_CALLBACK;

		WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
		if (state != nullptr && keycode != GLFW_KEY_UNKNOWN)
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressEvent event(static_cast<Keycode>(keycode), glfwModsToGBCMods(mods));
					state->eventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyRepeatEvent event(static_cast<Keycode>(keycode), glfwModsToGBCMods(mods));
					state->eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleaseEvent event(static_cast<Keycode>(keycode), glfwModsToGBCMods(mods));
					state->eventCallback(event);
					break;
				}
			}
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::CharCallback(GLFWwindow* window, uint32_t codepoint)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			KeyCharEvent event(codepoint);
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::CharModsCallback(GLFWwindow* window, uint32_t codepoint, int32_t mods)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			KeyCharModsEvent event(codepoint, glfwModsToGBCMods(mods));
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}


	void WindowsWindow::MouseButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
	{
		BEGIN_EVENT_CALLBACK;

		WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
		if (state != nullptr)
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressEvent event(static_cast<MouseButton>(button), glfwModsToGBCMods(mods));
					state->eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleaseEvent event(static_cast<MouseButton>(button), glfwModsToGBCMods(mods));
					state->eventCallback(event);
					break;
				}
			}
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::CursorPosCallback(GLFWwindow* window, double x, double y)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			MouseMoveEvent event(static_cast<float>(x), static_cast<float>(y));
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::ScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			MouseScrollEvent event(static_cast<float>(offsetX), static_cast<float>(offsetY));
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}

	void WindowsWindow::CursorEnterCallback(GLFWwindow* window, int32_t entered)
	{
		BEGIN_EVENT_CALLBACK;

		if (WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window)); state != nullptr)
		{
			MouseEnterEvent event(entered == GLFW_TRUE);
			state->containsMouse = event.HasEntered();
			state->eventCallback(event);
		}

		END_EVENT_CALLBACK;
	}
}
