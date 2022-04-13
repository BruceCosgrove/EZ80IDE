#pragma once

#include "GBC/Core/Window.h"

struct GLFWwindow;

namespace gbc
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowSpecifications& specification);
		virtual ~WindowsWindow();

		virtual void PollEvents() override;
		virtual void SwapBuffers() override;

		virtual int32_t GetWidth() const override { return state.current.size.x; }
		virtual int32_t GetHeight() const override { return state.current.size.y; }
		virtual int32_t GetX() const override { return state.current.position.x; }
		virtual int32_t GetY() const override { return state.current.position.y; }

		virtual bool IsFocused() const override { return state.focused; };
		virtual bool ContainsMouse() const override { return state.containsMouse; }
		virtual bool IsMinimized() const override { return state.minimized; }
		virtual bool IsMaximized() const override { return state.maximized; }

		virtual const std::string& GetTitle() const override { return state.title; }
		virtual void SetTitle(const std::string& title) override;

		virtual void SetIcon(const Ref<LocalTexture2D>& texture) override;

		virtual bool IsVSync() const override { return state.vsync; }
		virtual void SetVSync(bool vsync) override;

		virtual bool IsResizable() const override { return state.resizable; }
		virtual void SetResizable(bool resizable) override;

		virtual bool IsFullscreen() const override { return state.fullscreen; }
		virtual void SetFullscreen(bool fullscreen) override;

		virtual bool IsCaptureMouse() const override { return state.captureMouse; }
		virtual void SetCaptureMouse(bool captureMouse) override;

		virtual void* GetNativeWindow() override { return window; }
		virtual Context& GetContext() override { return *context; }

		virtual void SetEventCallback(const EventCallbackFunc& callback) override { state.eventCallback = callback; }
	private:
		void SaveDimensions();

		GLFWwindow* window = nullptr;
		Scope<Context> context = nullptr;

		struct WindowState
		{
			struct
			{
				glm::ivec2 position{ 0 };
				glm::ivec2 size{ 0 };
			} current, preFullscreen;

			union
			{
				uint8_t flags = 0;
				struct
				{
					char vsync : 1;
					char resizable : 1;
					char fullscreen : 1;
					char captureMouse : 1;

					char focused : 1;
					char minimized : 1;
					char maximized : 1;
					char containsMouse : 1;
				};
			};

			std::string title;
			EventCallbackFunc eventCallback;
		} state;

		// Callbacks
		static void SetCallbacks(GLFWwindow* window);

		static void WindowCloseCallback(GLFWwindow* window);
		static void WindowSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
		static void WindowPosCallback(GLFWwindow* window, int32_t x, int32_t y);
		static void WindowFocusCallback(GLFWwindow* window, int32_t focused);
		static void WindowIconifyCallback(GLFWwindow* window, int32_t iconified);
		static void WindowMaximizeCallback(GLFWwindow* window, int32_t maximized);
		static void DropCallback(GLFWwindow* window, int32_t pathCount, const char** paths);
		static void FramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
		static void WindowContentScaleCallback(GLFWwindow* window, float scaleX, float scaleY);
		static void WindowRefreshCallback(GLFWwindow* window);

		static void KeyCallback(GLFWwindow* window, int32_t keycode, int32_t scancode, int32_t action, int32_t mods);
		static void CharCallback(GLFWwindow* window, uint32_t codepoint);
		static void CharModsCallback(GLFWwindow* window, uint32_t codepoint, int32_t mods);

		static void MouseButtonCallback(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);
		static void CursorPosCallback(GLFWwindow* window, double x, double y);
		static void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);
		static void CursorEnterCallback(GLFWwindow* window, int32_t entered);
	};
}
