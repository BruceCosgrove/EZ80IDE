#include <GBC/Core/EntryPoint.h>
#include "Layers/EZ80IDELayer.h"

namespace gbc
{
	Application* CreateApplication(CommandLineArgs args)
	{
		WindowSpecifications specs;
		specs.maximizeOnShow = true;
		specs.title = "TI-84+CE EZ80 IDE";

		auto application = new Application(args, specs);
		application->PushLayer(new EZ80IDELayer());

		return application;
	}
}
