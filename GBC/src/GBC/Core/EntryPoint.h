#pragma once

#include "GBC/Core/Application.h"
#include "GBC/Core/Logger.h"
#include "GBC/Debug/Profiler.h"

#if GBC_PLATFORM_WINDOWS
namespace gbc
{
	extern Application* CreateApplication(CommandLineArgs args);

	int Main(CommandLineArgs args)
	{
		Logger::Init();

		GBC_PROFILE_BEGIN("Init", "GBCProfileInit.json");
		auto application = CreateApplication(args);
		GBC_PROFILE_END();

		GBC_PROFILE_BEGIN("Run", "GBCProfileRun.json");
		application->Run();
		GBC_PROFILE_END();

		GBC_PROFILE_BEGIN("Shutdown", "GBCProfileShutdown.json");
		delete application;
		GBC_PROFILE_END();

		return 0;
	}
}

#if GBC_CONFIG_DIST
#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNUSED_PARAMS(hInst, hInstPrev, lpCmdLine, nCmdShow);
	return gbc::Main({ __argc, __argv });
}

#else // !CONFIG_DIST

int main(int argc, char** argv)
{
	return gbc::Main({ argc, argv });
}

#endif // CONFIG_DIST

#endif // GBC_PLATFORM_WINDOWS
