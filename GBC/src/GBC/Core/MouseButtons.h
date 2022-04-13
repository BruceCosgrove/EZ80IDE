#pragma once

#include <cstdint>

// Taken from glfw3.h and modified

namespace gbc
{
	enum class MouseButton : uint16_t
	{
		Button1 = 0, ButtonLeft     = Button1,
		Button2 = 1, ButtonRight    = Button2,
		Button3 = 2, ButtonMiddle   = Button3,
		Button4 = 3, ButtonBackward = Button4,
		Button5 = 4, ButtonForward  = Button5,
		Button6 = 5,
		Button7 = 6,
		Button8 = 7,

		Count
	};
}
