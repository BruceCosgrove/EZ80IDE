#include "ExplorerPanel.h"
#if GBC_ENABLE_IMGUI
#include "Layers/EZ80IDELayer.h"
#include "Panels/Dist/FilePanel.h"
#include "GBC/ImGui/ImGuiHelper.h"
#include "GBC/IO/FileIO.h"

namespace gbc
{
	ExplorerPanel::ExplorerPanel(EZ80IDELayer* ez80IDELayer, const std::string& title)
		: Panel(ez80IDELayer, title) {}

	void ExplorerPanel::OnImGuiRender(int)
	{
		auto& ide = GetIDE();

		if (refreshDirectories)
		{
			refreshDirectories = false;
			RefreshDirectories();
		}

		if (ide.HasSubStates(IDEState_WorkspaceOpen))
		{
			bool clickedFile = false;
			RenderDirectory(workspaceDirectory, clickedFile);
			// TODO: check if IsMouseClicked needs to be if the window was clicked
			if (!ide.HasSubStates(IDEState_PopupOpen) && !clickedFile && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				selectedFilepath = workspaceDirectory.filepath;
				selectedFilepathIsDirectory = true;
				selectedFilepathDeleteAllowed = false;
			}

			if (optionsPopupOpen)
			{
				if (!ide.OpenPopup(optionsPopupTitle))
					optionsPopupOpen = false;
				else if (ImGui::BeginPopup(optionsPopupTitle))
				{
					if (selectedFilepathIsDirectory)
					{
						if (ImGui::MenuItem("New File", "Ctrl+N"))
							ide.NewFile();
						if (ImGui::MenuItem("New Folder", "Ctrl+Shift+N"))
							ide.NewFolder();
						if (ImGui::MenuItem("Open in Terminal"))
							FileDialog::OpenFolderInTerminal(selectedFilepath);
						if (ImGui::MenuItem("Open in File Explorer"))
							FileDialog::OpenFolderInExplorer(selectedFilepath);

						ImGui::Separator();

						// TODO: emulate cut/copy/paste
						if (ImGui::MenuItem("Cut", "Ctrl+X"))
							GBC_INFO("TODO: Directory: Cut");
						if (ImGui::MenuItem("Copy", "Ctrl+C"))
							GBC_INFO("TODO: Directory: Copy");
						// Can only paste file or directory into another directory.
						// Can't cut into the same directory, nor any of its subdirectories.
						if (ImGui::MenuItem("Paste", "Ctrl+V"))
							GBC_INFO("TODO: Directory: Paste");
					}
					else
					{
						if (ImGui::MenuItem("Open in Editor"))
							ide.FocusOrAddFilePanel(selectedFilepath);
						if (ImGui::MenuItem("Open in File Explorer"))
							FileDialog::OpenFileInExplorer(selectedFilepath);

						ImGui::Separator();

						// TODO: emulate cut/copy/paste
						if (ImGui::MenuItem("Cut", "Ctrl+X"))
							GBC_INFO("TODO: File: Cut");
						if (ImGui::MenuItem("Copy", "Ctrl+C"))
							GBC_INFO("TODO: File: Copy");
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Copy Path", "Shift+C"))
						ide.CopyPath(selectedFilepath);
					if (ImGui::MenuItem("Copy Relative Path", "Shift+Alt+C"))
						ide.CopyRelativePath(selectedFilepath);

					ImGui::Separator();

					if (ImGui::MenuItem("Rename", "Ctrl+R"))
						ide.Rename();
					if (ImGui::MenuItem("Delete", "Delete", false, selectedFilepathDeleteAllowed))
						ide.Delete();

					ImGui::EndPopup();
				}
			}
		}
	}

	void ExplorerPanel::RenderDirectory(const Directory& directory, bool& clickedFile)
	{
		auto& ide = GetIDE();

		ImGuiTreeNodeFlags directoryFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (!FileIO::IsAncestorOf(ide.srcDirectory, directory.filepath))
			directoryFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (directory.subDirectories.empty() && directory.files.empty())
			directoryFlags |= ImGuiTreeNodeFlags_Leaf;
		if (selectedFilepath == directory.filepath)
			directoryFlags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx(directory.filename.c_str(), directoryFlags);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			selectedFilepath = directory.filepath;
			selectedFilepathIsDirectory = true;
			selectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.srcDirectory, selectedFilepath);
			clickedFile = true;
		}

		if (!ide.IsAnyPopupOpen() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			selectedFilepath = directory.filepath;
			selectedFilepathIsDirectory = true;
			selectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.srcDirectory, selectedFilepath);
			clickedFile = true;

			optionsPopupOpen = true;
		}

		if (opened)
		{
			for (auto& subDirectory : directory.subDirectories)
				RenderDirectory(subDirectory, clickedFile);

			for (auto& file : directory.files)
			{
				ImGuiTreeNodeFlags fileFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				if (selectedFilepath == file.filepath)
					fileFlags |= ImGuiTreeNodeFlags_Selected;

				opened = ImGui::TreeNodeEx(file.filename.c_str(), fileFlags);

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					selectedFilepath = file.filepath;
					selectedFilepathIsDirectory = false;
					selectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.srcDirectory, selectedFilepath);
					clickedFile = true;

					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						ide.FocusOrAddFilePanel(file.filepath);
				}

				if (!ide.IsAnyPopupOpen() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					selectedFilepath = file.filepath;
					selectedFilepathIsDirectory = false;
					selectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.srcDirectory, selectedFilepath);
					clickedFile = true;

					optionsPopupOpen = true;
				}

				if (opened)
					ImGui::TreePop();
			}

			ImGui::TreePop();
		}
	}

	void ExplorerPanel::SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectoryFilepath)
	{
		if (workspaceDirectoryFilepath.empty())
		{
			workspaceDirectory.filepath.clear();
			workspaceDirectory.filename.clear();
			workspaceDirectory.subDirectories.clear();
			workspaceDirectory.files.clear();
			selectedFilepath.clear();
		}
		else
		{
			selectedFilepath = workspaceDirectoryFilepath;
			workspaceDirectory.filepath = workspaceDirectoryFilepath;
			workspaceDirectory.filename = workspaceDirectoryFilepath.filename().string();
			RefreshDirectories();

			notifier = DirectoryChange::Notifier(
				GBC_BIND_FUNC(OnDirectoryNotification),
				workspaceDirectory.filepath,
				DirectoryChange::NotificationType_NameChanged,
				true
			);
		}
	}

	std::filesystem::path ExplorerPanel::GetNewFileDirectory() const
	{
		auto& ide = GetIDE();

		if (!FileIO::IsAncestorOf(ide.srcDirectory, selectedFilepath))
			return ide.srcDirectory;
		return selectedFilepathIsDirectory ? selectedFilepath : selectedFilepath.parent_path();
	}

	void ExplorerPanel::RefreshDirectories()
	{
		auto& ide = GetIDE();

		workspaceDirectory.subDirectories.clear();
		workspaceDirectory.files.clear();
		RefreshDirectory(workspaceDirectory);

		if (!selectedFilepath.empty())
		{
			while (!FileIO::DirectoryExists(selectedFilepath) && FileIO::IsAncestorOf(workspaceDirectory.filepath, selectedFilepath))
				selectedFilepath = selectedFilepath.parent_path();
			if (!FileIO::IsAncestorOf(workspaceDirectory.filepath, selectedFilepath))
				selectedFilepath = workspaceDirectory.filepath;
		}
		if (optionsPopupOpen)
		{
			if (!FileIO::DirectoryExists(selectedFilepath))
			{
				selectedFilepath.clear();
				ide.ClosePopup(optionsPopupTitle);
				optionsPopupOpen = false;
			}
		}
	}

	void ExplorerPanel::RefreshDirectory(Directory& directory)
	{
		std::error_code error;
		for (const auto& entry : std::filesystem::directory_iterator(directory.filepath, error))
		{
			if (!FileIO::IsHidden(entry.path()))
			{
				if (entry.is_directory(error))
					RefreshDirectory(directory.subDirectories.emplace_back(entry.path()));
				else
					directory.files.emplace_back(entry.path());
			}
		}
	}

	bool ExplorerPanel::OnDirectoryNotification(bool error)
	{
		if (error)
		{
			GBC_CORE_ERROR("Explorer directory notification failed! Save and restart highly recommended.");
			return false; // return value here does not matter as it is not used
		}

		refreshDirectories = true;
		return true;
	}
}
#endif
