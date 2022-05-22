#include "RendererInfoPanel.h"
#if GBC_ENABLE_STATS
#include "GBC/Core/Application.h"
#include "GBC/ImGui/ImGuiHelper.h"

namespace gbc
{
	RendererInfoPanel::RendererInfoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title)
	{
		const auto& context = Application::Get().GetWindow().GetContext();
		vendor = context.GetVendor();
		renderer = context.GetRenderer();
		version = context.GetVersion();
	}

	void RendererInfoPanel::OnImGuiRender(int)
	{
		if (ImGuiHelper::BeginTable("RendererInfo"))
		{
			ImGuiHelper::Text("Vendor", vendor);
			ImGuiHelper::NextTableColumn();
			ImGuiHelper::Text("Renderer", renderer);
			ImGuiHelper::NextTableColumn();
			ImGuiHelper::Text("Version", version);
			ImGuiHelper::EndTable();
		}
	}
}
#endif
