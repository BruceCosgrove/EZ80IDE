#include "ProfilingPanel.h"
#if GBC_ENABLE_PROFILE_RUNTIME
#include "GBC/Debug/Profiler.h"
#include "GBC/ImGui/ImGuiHelper.h"

namespace gbc
{
	ProfilingPanel::ProfilingPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title) {}

	void ProfilingPanel::OnImGuiRender(int)
	{
		if (ImGuiHelper::BeginTable("Profiling", 2))
		{
			if (ImGuiHelper::Checkbox(profiling ? "Stop Profiling" : "Start Profiling", &profiling))
			{
				if (profiling)
					GBC_PROFILE_BEGIN_RUNTIME("Runtime", (std::wstring(L"ProfileSessions/GBCProfileRuntime") += std::to_wstring(++profileCount)) += L".json");
				else
					GBC_PROFILE_END_RUNTIME();
			}

			ImGuiHelper::EndTable();
		}
	}
}
#endif
