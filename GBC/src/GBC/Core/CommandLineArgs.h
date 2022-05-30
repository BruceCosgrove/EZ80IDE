#pragma once

#include "GBC/Core/Assert.h"

namespace gbc
{
	struct CommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		constexpr const char* operator[](int index) const
		{
			GBC_CORE_ASSERT(0 <= index && index < count, "Command Line Args index out of bounds!");
			return args[index];
		}
	};

	int Main(CommandLineArgs args);
}
