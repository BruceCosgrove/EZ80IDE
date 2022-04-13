#include "gbcpch.h"
#if GBC_ENABLE_LOGGING
#include "MouseEvents.h"

namespace gbc
{
	std::string MouseButtonPressEvent::ToString() const
	{
		std::stringstream stream;
		stream << "MouseButtonPressEvent: button=" << static_cast<uint16_t>(button) << " mods=" << mods;
		return stream.str();
	}

	std::string MouseButtonReleaseEvent::ToString() const
	{
		std::stringstream stream;
		stream << "MouseButtonReleaseEvent: button=" << static_cast<uint16_t>(button) << " mods=" << mods;
		return stream.str();
	}

	std::string MouseMoveEvent::ToString() const
	{
		std::stringstream stream;
		stream << "MouseMoveEvent: x=" << x << " y=" << y;
		return stream.str();
	}

	std::string MouseScrollEvent::ToString() const
	{
		std::stringstream stream;
		stream << "MouseScrollEvent: offsetX=" << offsetX << " offsetY=" << offsetY;
		return stream.str();
	}

	std::string MouseEnterEvent::ToString() const
	{
		std::stringstream stream;
		stream << "MouseEnterEvent: enter=" << enter;
		return stream.str();
	}
}
#endif
