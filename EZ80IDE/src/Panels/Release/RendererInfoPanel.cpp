#include "RendererInfoPanel.h"
#if GBC_ENABLE_STATS
#include "GBC/Core/Application.h"
#include "GBC/ImGui/ImGuiHelper.h"

namespace ide
{
	RendererInfoPanel::RendererInfoPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title)
	{
		const auto& context = gbc::Application::Get().GetWindow().GetContext();
		m_pVendor = context.GetVendor();
		m_pRenderer = context.GetRenderer();
		m_pVersion = context.GetVersion();
	}

	void RendererInfoPanel::OnImGuiRender(int)
	{
		if (gbc::ImGuiHelper::BeginTable("RendererInfo"))
		{
			gbc::ImGuiHelper::Text("Vendor", m_pVendor);
			gbc::ImGuiHelper::NextTableColumn();
			gbc::ImGuiHelper::Text("Renderer", m_pRenderer);
			gbc::ImGuiHelper::NextTableColumn();
			gbc::ImGuiHelper::Text("Version", m_pVersion);
			gbc::ImGuiHelper::EndTable();
		}
	}
}
#endif
