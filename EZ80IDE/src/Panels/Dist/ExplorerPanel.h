#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
#include "GBC/Events/KeyEvents.h"
#include "GBC/IO/DirectoryChange.h"
#include "Panels/Panel.h"
#include <filesystem>

namespace gbc
{
	class ExplorerPanel : public Panel
	{
	public:
		ExplorerPanel(EZ80IDELayer* ez80IDELayer, const std::string& title);

		virtual void OnImGuiRender(int) override;

		void SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectoryFilepath);

		const std::filesystem::path& GetSelectedFilepath() const noexcept { return selectedFilepath; }
		bool IsSelectedFilepathADirectory() const noexcept { return selectedFilepathIsDirectory; }
		bool IsSelectedFilepathDeleteAllowed() const noexcept { return selectedFilepathDeleteAllowed; }
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
		bool refreshDirectories = false;

		Directory workspaceDirectory;
		std::filesystem::path selectedFilepath;
		bool selectedFilepathIsDirectory = true;
		bool selectedFilepathDeleteAllowed = false;
	private:
		DirectoryChange::Notifier notifier;
		bool OnDirectoryNotification(bool error);
	private:
		static constexpr char optionsPopupTitle[] = "ExplorerOptionsPopup";
		static constexpr char renamePopupTitle[] = "ExplorerRenamePopup";
		bool optionsPopupOpen = false;
	};
}
#endif
