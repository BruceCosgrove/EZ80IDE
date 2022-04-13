#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
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
		inline virtual const std::string& GetTitle() const override { return windowTitle; }
		inline const std::filesystem::path& GetFilepath() const noexcept { return filepath; }
		void SetFilepath(const std::filesystem::path& filepath) noexcept;

		virtual void SetEnabled(bool enabled) noexcept override;
		virtual void SetFocused(bool focused) noexcept override;

		void OnChanged();
		inline void MarkForCloseWithoutSaving() noexcept { unsavedChanges = false; }
		void Save();
	private:
		void UpdateWindowTitle();
	private:
		std::filesystem::path filepath;
		std::string windowTitle;
		bool unsavedChanges = false;
		size_t asteriskIndex;
	public:
		static constexpr char identifier[] = "###file";
		static constexpr size_t identifierLength = sizeof(identifier) / sizeof(*identifier) - 1;
	};
}
#endif
