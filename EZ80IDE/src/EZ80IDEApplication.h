#pragma once

#include "GBC/Core/Application.h"

namespace gbc
{
	class EZ80IDEApplication : public Application
	{
	public:
		EZ80IDEApplication(CommandLineArgs args, const WindowSpecifications& specs);
	};
}
