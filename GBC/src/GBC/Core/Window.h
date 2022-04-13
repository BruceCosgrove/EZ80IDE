#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Events/Event.h"
#include "GBC/Rendering/Context.h"
#include "GBC/Rendering/LocalTexture2D.h"
#include <functional>
#include <string>

namespace gbc
{
	struct WindowSpecifications
	{
		int32_t width = 1600;
		int32_t height = 900;
		std::string title = "GBC v1.3";

		bool vsync = true;
		bool resizable = true;
		bool fullscreen = false;
		bool captureMouse = false;

		bool focusOnShow = true;
		bool maximizeOnShow = false;
	};

	class Window
	{
	public:
		virtual ~Window() = default;

		virtual void PollEvents() = 0;
		virtual void SwapBuffers() = 0;

		virtual int32_t GetWidth() const = 0;
		virtual int32_t GetHeight() const = 0;
		virtual int32_t GetX() const = 0;
		virtual int32_t GetY() const = 0;

		virtual bool IsFocused() const = 0;
		virtual bool ContainsMouse() const = 0;
		virtual bool IsMinimized() const = 0;
		virtual bool IsMaximized() const = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual void SetTitle(const std::string& title) = 0;

		virtual void SetIcon(const Ref<LocalTexture2D>& texture) = 0;

		virtual bool IsVSync() const = 0;
		virtual void SetVSync(bool vsync) = 0;
		constexpr void ToggleVSync() { SetVSync(!IsVSync()); }

		virtual bool IsResizable() const = 0;
		virtual void SetResizable(bool resizable) = 0;
		constexpr void ToggleResizable() { SetResizable(!IsResizable()); }

		virtual bool IsFullscreen() const = 0;
		virtual void SetFullscreen(bool fullscreen) = 0;
		constexpr void ToggleFullscreen() { SetFullscreen(!IsFullscreen()); }

		virtual bool IsCaptureMouse() const = 0;
		virtual void SetCaptureMouse(bool captureMouse) = 0;
		constexpr void ToggleCaptureMouse() { SetCaptureMouse(!IsCaptureMouse()); }

		virtual void* GetNativeWindow() = 0;
		virtual Context& GetContext() = 0;

		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;

		static Scope<Window> Create(const WindowSpecifications& specification);
	};
}
