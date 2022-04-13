#include "gbcpch.h"
#if GBC_ENABLE_LOGGING
#include "KeyEvents.h"

namespace gbc
{
	std::string KeyPressEvent::ToString() const
	{
		std::stringstream stream;
		stream << "KeyPressEvent: keycode=" << static_cast<uint16_t>(keycode) << " mods=" << mods;
		return stream.str();
	}

	std::string KeyRepeatEvent::ToString() const
	{
		std::stringstream stream;
		stream << "KeyRepeatEvent: keycode=" << static_cast<uint16_t>(keycode) << " mods=" << mods;
		return stream.str();
	}

	std::string KeyReleaseEvent::ToString() const
	{
		std::stringstream stream;
		stream << "KeyReleaseEvent: keycode=" << static_cast<uint16_t>(keycode) << " mods=" << mods;
		return stream.str();
	}

	std::string KeyCharEvent::ToString() const
	{
		std::stringstream stream;
		stream << "KeyCharEvent: codepoint=" << codepoint;
		return stream.str();
	}

	std::string KeyCharModsEvent::ToString() const
	{
		std::stringstream stream;
		stream << "KeyCharEvent: codepoint=" << codepoint << " mods=" << mods;
		return stream.str();
	}
}
#endif
