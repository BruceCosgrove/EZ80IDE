#pragma once

#include "GBC/Core/Core.h"
#include <functional>
#if GBC_ENABLE_LOGGING
	#include <string>
#endif

namespace gbc
{
	enum class EventType
	{
		WindowClose, WindowResize, WindowMove, WindowFocus, WindowMinimize, WindowMaximize, 
			WindowDrop, WindowFramebufferResize, WindowContentScale, WindowRefresh,
		KeyPress, KeyRepeat, KeyRelease, KeyChar, KeyCharMods,
		MouseButtonPress, MouseButtonRelease, MouseMove, MouseScroll, MouseEnter,
		JoystickConnect
	};

	using EventCategoryFlags = uint32_t;
	enum EventCategory : EventCategoryFlags
	{
		EventCategory_None = 0,
		EventCategory_Application = 1 << 0,
		EventCategory_Window      = 1 << 1,
		EventCategory_Keyboard    = 1 << 2,
		EventCategory_Mouse       = 1 << 3,
		EventCategory_MouseButton = 1 << 4,
		EventCategory_Device      = 1 << 5
	};

#define EVENT_CLASS_TYPE(type) static constexpr EventType GetStaticType() noexcept { return type; }\
							   constexpr virtual EventType GetType() const noexcept override { return GetStaticType(); }

#define EVENT_CLASS_CATEGORY(categories) constexpr virtual EventCategoryFlags GetCategoryFlags() const noexcept override { return categories; }

	class Event
	{
	public:
		constexpr virtual EventType GetType() const noexcept = 0;
		constexpr virtual EventCategoryFlags GetCategoryFlags() const noexcept = 0;
		constexpr bool IsInCategory(EventCategoryFlags category) const noexcept;

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const = 0;
#endif

		bool handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event) : event(event) {}
	public:
		template<typename C, typename E> bool Dispatch(C* object, void(C::*func)(E&));
		template<typename E> bool Dispatch(void(*func)(E&));
		template<typename C> bool Dispatch(C* object, void(C::*func)(Event&));
		constexpr void Dispatch(void(*func)(Event&));
	private:
		Event& event;
	};

	using EventCallbackFunc = std::function<void(Event&)>;
}

#include "Event.inl"
