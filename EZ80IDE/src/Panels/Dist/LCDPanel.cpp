#include "LCDPanel.h"
#include "Layers/EZ80IDELayer.h"
#include "CEmu/core/lcd.h"

namespace ide::emu
{
	LCDPanel::LCDPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title)
	{
		m_LCDTexture = gbc::Texture2D::Create(gbc::LocalTexture2D::Create(LCD_WIDTH, LCD_HEIGHT, 4, true));
	}

	void LCDPanel::OnImGuiRender(int)
	{
		auto& ide = GetIDE();
		if (ide.HasSubStates(IDEState_ROMLoaded))
		{
			auto& emulatorThread = ide.GetEmulatorThread();

			if (emulatorThread.ShouldLCDUpdate())
				m_LCDTexture->Update();

			auto& localTexture = m_LCDTexture->GetLocalTexture();
			ImVec2 size{ static_cast<float>(localTexture->GetWidth()), static_cast<float>(localTexture->GetHeight()) };
			ImGui::Image(m_LCDTexture->GetRendererID(), size);
		}
	}
}
