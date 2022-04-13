#pragma once

#include "GBC/Events/Event.h"

namespace gbc
{
	class WindowEvent : public Event
	{
	public:
		constexpr WindowEvent(void* nativeWindow) : nativeWindow(nativeWindow) {}

		constexpr void* GetNativeWindow() const noexcept { return nativeWindow; }
	private:
		void* nativeWindow;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		constexpr WindowCloseEvent(void* nativeWindow) : WindowEvent(nativeWindow) {}
		EVENT_CLASS_TYPE(EventType::WindowClose);
		EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	};

	class WindowResizeEvent : public WindowEvent
	{
	public:
		constexpr WindowResizeEvent(void* nativeWindow, int32_t width, int32_t height) : WindowEvent(nativeWindow), width(width), height(height) {}
		EVENT_CLASS_TYPE(EventType::WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);

		constexpr int32_t GetWidth() const noexcept { return width; }
		constexpr int32_t GetHeight() const noexcept { return height; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		int32_t width;
		int32_t height;
	};

	class WindowMoveEvent : public WindowEvent
	{
	public:
		constexpr WindowMoveEvent(void* nativeWindow, int32_t x, int32_t y) : WindowEvent(nativeWindow), x(x), y(y) {}
		EVENT_CLASS_TYPE(EventType::WindowMove);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

		constexpr int32_t GetX() const noexcept { return x; }
		constexpr int32_t GetY() const noexcept { return y; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		int32_t x;
		int32_t y;
	};

	class WindowFocusEvent : public WindowEvent
	{
	public:
		constexpr WindowFocusEvent(void* nativeWindow, bool focus) : WindowEvent(nativeWindow), focus(focus) {}
		EVENT_CLASS_TYPE(EventType::WindowFocus);
		EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);

		constexpr bool IsFocused() const noexcept { return focus; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		bool focus;
	};

	class WindowMinimizeEvent : public WindowEvent
	{
	public:
		constexpr WindowMinimizeEvent(void* nativeWindow, bool minimize) : WindowEvent(nativeWindow), minimize(minimize) {}
		EVENT_CLASS_TYPE(EventType::WindowMinimize);
		EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);

		constexpr bool IsMinimized() const noexcept { return minimize; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		bool minimize;
	};

	class WindowMaximizeEvent : public WindowEvent
	{
	public:
		constexpr WindowMaximizeEvent(void* nativeWindow, bool maximize) : WindowEvent(nativeWindow), maximize(maximize) {}
		EVENT_CLASS_TYPE(EventType::WindowMaximize);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

		constexpr bool IsMaximized() const noexcept { return maximize; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		bool maximize;
	};

	class WindowDropEvent : public WindowEvent
	{
	public:
		constexpr WindowDropEvent(void* nativeWindow, uint32_t pathCount, const char** paths) : WindowEvent(nativeWindow), pathCount(pathCount), paths(paths) {}
		EVENT_CLASS_TYPE(EventType::WindowDrop);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

		constexpr uint32_t GetPathCount() const noexcept { return pathCount; }
		constexpr const char*const *const GetPaths() const noexcept { return paths; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		uint32_t pathCount;
		const char** paths;
	};

	class WindowFramebufferResizeEvent : public WindowEvent
	{
	public:
		constexpr WindowFramebufferResizeEvent(void* nativeWindow, int32_t width, int32_t height) : WindowEvent(nativeWindow), width(width), height(height) {}
		EVENT_CLASS_TYPE(EventType::WindowFramebufferResize);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

		constexpr int32_t GetWidth() const noexcept { return width; }
		constexpr int32_t GetHeight() const noexcept { return height; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		int32_t width;
		int32_t height;
	};

	class WindowContentScaleEvent : public WindowEvent
	{
	public:
		constexpr WindowContentScaleEvent(void* nativeWindow, float scaleX, float scaleY) : WindowEvent(nativeWindow), scaleX(scaleX), scaleY(scaleY) {}
		EVENT_CLASS_TYPE(EventType::WindowContentScale);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

		constexpr float GetScaleX() const noexcept { return scaleX; }
		constexpr float GetScaleY() const noexcept { return scaleY; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		float scaleX;
		float scaleY;
	};

	class WindowRefreshEvent : public WindowEvent
	{
	public:
		constexpr WindowRefreshEvent(void* nativeWindow) : WindowEvent(nativeWindow) {}
		EVENT_CLASS_TYPE(EventType::WindowRefresh);
		EVENT_CLASS_CATEGORY(EventCategory_Window);

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	};
}
