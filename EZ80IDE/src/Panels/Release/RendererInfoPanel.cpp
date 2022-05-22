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
		m_pVendor = context.GetVendor();
		m_pRenderer = context.GetRenderer();
		m_pVersion = context.GetVersion();
	}

	void RendererInfoPanel::OnImGuiRender(int)
	{
		if (ImGuiHelper::BeginTable("RendererInfo"))
		{
			ImGuiHelper::Text("Vendor", m_pVendor);
			ImGuiHelper::NextTableColumn();
			ImGuiHelper::Text("Renderer", m_pRenderer);
			ImGuiHelper::NextTableColumn();
			ImGuiHelper::Text("Version", m_pVersion);
			ImGuiHelper::EndTable();
		}
	}
}
#endif
