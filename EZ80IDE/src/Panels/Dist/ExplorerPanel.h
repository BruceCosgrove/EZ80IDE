#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_IMGUI
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
	private:
		DirectoryChange::Notifier notifier;
		bool OnDirectoryNotification(bool error);
	private:
		static constexpr char optionsPopupTitle[] = "ExplorerOptionsPopup";
		static constexpr char renamePopupTitle[] = "ExplorerRenamePopup";
		bool optionsDeleteAllowed = false;
		bool optionsPopupOpen = false;
		bool renamePopupOpen = false;
	};
}
#endif
