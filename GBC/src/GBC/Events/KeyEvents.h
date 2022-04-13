#pragma once

#include "GBC/Events/Event.h"
#include "GBC/Events/Mods.h"
#include "GBC/Core/Keycodes.h"

namespace gbc
{
	class KeyPressEvent : public Event
	{
	public:
		constexpr KeyPressEvent(Keycode keycode, Mods mods) : keycode(keycode), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::KeyPress);
		EVENT_CLASS_CATEGORY(EventCategory_Keyboard);

		constexpr Keycode GetKeycode() const noexcept { return keycode; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		Keycode keycode;
		Mods mods;
	};

	class KeyRepeatEvent : public Event
	{
	public:
		constexpr KeyRepeatEvent(Keycode keycode, Mods mods) : keycode(keycode), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::KeyRepeat);
		EVENT_CLASS_CATEGORY(EventCategory_Keyboard);

		constexpr Keycode GetKeycode() const noexcept { return keycode; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		Keycode keycode;
		Mods mods;
	};

	class KeyReleaseEvent : public Event
	{
	public:
		constexpr KeyReleaseEvent(Keycode keycode, Mods mods) : keycode(keycode), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::KeyRelease);
		EVENT_CLASS_CATEGORY(EventCategory_Keyboard);

		constexpr Keycode GetKeycode() const noexcept { return keycode; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		Keycode keycode;
		Mods mods;
	};

	class KeyCharEvent : public Event
	{
	public:
		constexpr KeyCharEvent(uint32_t codepoint) : codepoint(codepoint) {}
		EVENT_CLASS_TYPE(EventType::KeyChar);
		EVENT_CLASS_CATEGORY(EventCategory_Keyboard);

		constexpr uint32_t GetCodepoint() const noexcept { return codepoint; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		uint32_t codepoint;
	};

	class KeyCharModsEvent : public Event
	{
	public:
		constexpr KeyCharModsEvent(uint32_t codepoint, Mods mods) : codepoint(codepoint), mods(mods) {}
		EVENT_CLASS_TYPE(EventType::KeyCharMods);
		EVENT_CLASS_CATEGORY(EventCategory_Keyboard);

		constexpr uint32_t GetCodepoint() const noexcept { return codepoint; }
		constexpr Mods GetMods() const noexcept { return mods; }

#if GBC_ENABLE_LOGGING
		virtual std::string ToString() const override;
#endif
	private:
		uint32_t codepoint;
		Mods mods;
	};
}
