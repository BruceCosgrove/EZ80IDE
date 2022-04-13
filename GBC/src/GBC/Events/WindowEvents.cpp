#include "gbcpch.h"
#if GBC_ENABLE_LOGGING
#include "WindowEvents.h"

namespace gbc
{
	std::string WindowCloseEvent::ToString() const
	{
		return "WindowCloseEvent";
	}

	std::string WindowResizeEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowResizeEvent: width=" << width << " height=" << height;
		return stream.str();
	}

	std::string WindowMoveEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowMoveEvent: x=" << x << " y=" << y;
		return stream.str();
	}

	std::string WindowFocusEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowFocusEvent: focused=" << focus;
		return stream.str();
	}

	std::string WindowMinimizeEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowMinimizeEvent: minimized=" << minimize;
		return stream.str();
	}

	std::string WindowMaximizeEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowMaximizeEvent: maximized=" << maximize;
		return stream.str();
	}

	std::string WindowDropEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowDropEvent: pathCount=" << pathCount << " paths:";
		for (uint32_t i = 0; i < pathCount; i++)
			stream << " \"" << paths[i] << '"';
		return stream.str();
	}

	std::string WindowFramebufferResizeEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowFramebufferResizeEvent: width=" << width << " height=" << height;
		return stream.str();
	}

	std::string WindowContentScaleEvent::ToString() const
	{
		std::stringstream stream;
		stream << "WindowContentScaleEvent: scaleX=" << scaleX << " scaleY=" << scaleY;
		return stream.str();
	}

	std::string WindowRefreshEvent::ToString() const
	{
		return "WindowContentScaleEvent";
	}
}
#endif
