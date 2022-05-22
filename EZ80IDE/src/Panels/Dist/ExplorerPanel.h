#pragma once

#include "GBC/Core/Core.h"
#include "GBC/Events/KeyEvents.h"
#include "GBC/IO/DirectoryChange.h"
#include "Panels/Panel.h"
#include <filesystem>

namespace ide
{
	class ExplorerPanel : public Panel
	{
	public:
		ExplorerPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;

		void SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectoryFilepath);

		const std::filesystem::path& GetSelectedFilepath() const noexcept { return m_SelectedFilepath; }
		bool IsSelectedFilepathADirectory() const noexcept { return m_SelectedFilepathIsDirectory; }
		bool IsSelectedFilepathDeleteAllowed() const noexcept { return m_SelectedFilepathDeleteAllowed; }
		std::filesystem::path GetNewFileDirectory() const;
	private:
		struct File
		{
			constexpr File() noexcept = default;
			File(const std::filesystem::path& filepath)
				: filepath(filepath), filename(filepath.filename().string()) {}

			std::filesystem::path filepath;
			std::string filename;
		};

		struct Directory : public File
		{
			constexpr Directory() noexcept = default;
			Directory(const std::filesystem::path& filepath)
				: File(filepath) {}

			std::vector<Directory> subDirectories;
			std::vector<File> files;
		};
	private:
		void RenderDirectory(const Directory& directory, bool& clickedFile);
	private:
		void RefreshDirectories();
		void RefreshDirectory(Directory& directory);
		bool m_RefreshDirectories = false;

		Directory m_WorkspaceDirectory;
		std::filesystem::path m_SelectedFilepath;
		bool m_SelectedFilepathIsDirectory = true;
		bool m_SelectedFilepathDeleteAllowed = false;
	private:
		gbc::DirectoryChange::Notifier m_Notifier;
		bool OnDirectoryNotification(bool error);
	private:
		static constexpr char s_pOptionsPopupTitle[] = "ExplorerOptionsPopup";
		static constexpr char s_pRenamePopupTitle[] = "ExplorerRenamePopup";
		bool m_OptionsPopupOpen = false;
	};
}
