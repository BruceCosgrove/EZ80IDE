#pragma once

#include "GBC/Events/Event.h"
#include "GBC/Events/Mods.h"
#include "GBC/Core/MouseButtons.h"

namespace gbc
{
	class MouseButtonPressEvent : public Event
	{
	public:
		constexpr MouseButtonPressEvent(MouseButton button, Mods mods) : button(button), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::MouseButtonPress);
		EVENT_CLASS_CATEGORY(EventCategory_Mouse | EventCategory_MouseButton);

		constexpr MouseButton GetButton() const noexcept { return button; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		MouseButton button;
		Mods mods;
	};

	class MouseButtonReleaseEvent : public Event
	{
	public:
		constexpr MouseButtonReleaseEvent(MouseButton button, Mods mods) : button(button), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::MouseButtonRelease);
		EVENT_CLASS_CATEGORY(EventCategory_Mouse | EventCategory_MouseButton);

		constexpr MouseButton GetButton() const noexcept { return button; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		MouseButton button;
		Mods mods;
	};

	class MouseMoveEvent : public Event
	{
	public:
		constexpr MouseMoveEvent(float x, float y) : x(x), y(y) {}
		EVENT_CLASS_TYPE(EventType::MouseMove);
		EVENT_CLASS_CATEGORY(EventCategory_Mouse);

		constexpr float GetX() const noexcept { return x; }
		constexpr float GetY() const noexcept { return y; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		float x;
		float y;
	};

	class MouseScrollEvent : public Event
	{
	public:
		constexpr MouseScrollEvent(float offsetX, float offsetY) : offsetX(offsetX), offsetY(offsetY) {}
		EVENT_CLASS_TYPE(EventType::MouseScroll);
		EVENT_CLASS_CATEGORY(EventCategory_Mouse);

		constexpr float GetOffsetX() const noexcept { return offsetX; }
		constexpr float GetOffsetY() const noexcept { return offsetY; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		float offsetX;
		float offsetY;
	};

	class MouseEnterEvent : public Event
	{
	public:
		constexpr MouseEnterEvent(bool enter) : enter(enter) {}
		EVENT_CLASS_TYPE(EventType::MouseEnter);
		EVENT_CLASS_CATEGORY(EventCategory_Mouse);

		constexpr bool HasEntered() const noexcept { return enter; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		bool enter;
	};
}
