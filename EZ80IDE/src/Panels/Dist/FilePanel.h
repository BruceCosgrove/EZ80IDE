#pragma once

#include "GBC/Core/Core.h"
#include "Panels/Panel.h"
#include <filesystem>

namespace gbc
{
	class FilePanel : public Panel
	{
	public:
		FilePanel(EZ80IDELayer* ez80IDELayer, const std::filesystem::path& filepath, uint64_t id);
		virtual ~FilePanel();

		virtual void OnImGuiRender(int) override;
	public:
		inline const std::filesystem::path& GetFilepath() const noexcept { return m_Filepath; }
		inline virtual const std::string& GetTitle() const override { return m_WindowTitle; }
		void Rename(const std::filesystem::path& filepath) noexcept;

		virtual void SetEnabled(bool enabled) noexcept override;
		virtual void SetFocused(bool focused) noexcept override;

		void OnChanged();
		inline void MarkForCloseWithoutSaving() noexcept { m_UnsavedChanges = false; }
		void Save();
	private:
		void UpdateWindowTitle();
	private:
		std::filesystem::path m_Filepath;
		std::string m_WindowTitle;
		bool m_UnsavedChanges = false;
		size_t m_AsteriskIndex;
	public:
		static constexpr char s_Identifier[] = "###file";
		static constexpr size_t m_IdentifierLength = sizeof(s_Identifier) / sizeof(*s_Identifier) - 1;
	};
}
