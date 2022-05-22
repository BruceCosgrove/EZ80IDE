#include "ProfilingPanel.h"
#if GBC_ENABLE_PROFILE_RUNTIME
#include "GBC/Debug/Profiler.h"
#include "GBC/ImGui/ImGuiHelper.h"

namespace ide
{
	ProfilingPanel::ProfilingPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title) {}

	void ProfilingPanel::OnImGuiRender(int)
	{
		if (gbc::ImGuiHelper::BeginTable("Profiling", 2))
		{
			if (gbc::ImGuiHelper::Checkbox(m_Profiling ? "Stop Profiling" : "Start Profiling", &m_Profiling))
			{
				if (m_Profiling)
					GBC_PROFILE_BEGIN_RUNTIME("Runtime", (std::wstring(L"ProfileSessions/GBCProfileRuntime") += std::to_wstring(++m_ProfileCount)) += L".json");
				else
					GBC_PROFILE_END_RUNTIME();
			}

			gbc::ImGuiHelper::EndTable();
		}
	}
}
#endif
