#include "DemoPanel.h"
#if GBC_CONFIG_DEBUG
#include <imgui/imgui.h>

namespace ide
{
	DemoPanel::DemoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title) {}

	void DemoPanel::OnImGuiRender(int)
	{
		bool enabled = IsEnabled();
		ImGui::ShowDemoWindow(&enabled);
		SetEnabled(enabled);
	}
}
#endif
