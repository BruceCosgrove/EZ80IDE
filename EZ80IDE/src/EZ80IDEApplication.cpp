#include "EZ80IDEApplication.h"
#include "Layers/EZ80IDELayer.h"
#include "GBC/Core/EntryPoint.h"

namespace gbc
{
	EZ80IDEApplication::EZ80IDEApplication(CommandLineArgs args, const WindowSpecifications& specs)
		: Application(args, specs)
	{
		PushLayer(new EZ80IDELayer());
	}

	Application* CreateApplication(CommandLineArgs args)
	{
		WindowSpecifications specs;
		specs.maximizeOnShow = true;
		specs.title = "EZ80 IDE";

		return new EZ80IDEApplication(args, specs);
	}
}
