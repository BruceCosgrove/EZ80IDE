#pragma once

#include "GBC/Rendering/Texture.h"
#include "Panels/Panel.h"

namespace ide::emu
{
	class LCDPanel : public Panel
	{
	public:
		LCDPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;
	public:
		inline const gbc::Ref<gbc::LocalTexture2D>& GetLocalLCDTexture() const { return m_LCDTexture->GetLocalTexture(); }
	private:
		gbc::Ref<gbc::Texture2D> m_LCDTexture;
	};
}
