#include "ExplorerPanel.h"
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

		if (m_RefreshDirectories)
		{
			m_RefreshDirectories = false;
			RefreshDirectories();
		}

		if (ide.HasSubStates(IDEState_WorkspaceOpen))
		{
			bool clickedFile = false;
			RenderDirectory(m_WorkspaceDirectory, clickedFile);
			// TODO: check if IsMouseClicked needs to be if the window was clicked
			if (!ide.HasSubStates(IDEState_PopupOpen) && !clickedFile && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedFilepath = m_WorkspaceDirectory.filepath;
				m_SelectedFilepathIsDirectory = true;
				m_SelectedFilepathDeleteAllowed = false;
			}

			if (m_OptionsPopupOpen)
			{
				if (!ide.OpenPopup(s_pOptionsPopupTitle))
					m_OptionsPopupOpen = false;
				else if (ImGui::BeginPopup(s_pOptionsPopupTitle))
				{
					if (m_SelectedFilepathIsDirectory)
					{
						if (ImGui::MenuItem("New File", "Ctrl+N"))
							ide.NewFile();
						if (ImGui::MenuItem("New Folder", "Ctrl+Shift+N"))
							ide.NewFolder();
						if (ImGui::MenuItem("Open in Terminal"))
							FileDialog::OpenFolderInTerminal(m_SelectedFilepath);
						if (ImGui::MenuItem("Open in File Explorer"))
							FileDialog::OpenFolderInExplorer(m_SelectedFilepath);

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
							ide.FocusOrAddFilePanel(m_SelectedFilepath);
						if (ImGui::MenuItem("Open in File Explorer"))
							FileDialog::OpenFileInExplorer(m_SelectedFilepath);

						ImGui::Separator();

						// TODO: emulate cut/copy/paste
						if (ImGui::MenuItem("Cut", "Ctrl+X"))
							GBC_INFO("TODO: File: Cut");
						if (ImGui::MenuItem("Copy", "Ctrl+C"))
							GBC_INFO("TODO: File: Copy");
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Copy Path", "Shift+C"))
						ide.CopyPath(m_SelectedFilepath);
					if (ImGui::MenuItem("Copy Relative Path", "Shift+Alt+C"))
						ide.CopyRelativePath(m_SelectedFilepath);

					ImGui::Separator();

					if (ImGui::MenuItem("Rename", "Ctrl+R"))
						ide.Rename();
					if (ImGui::MenuItem("Delete", "Delete", false, m_SelectedFilepathDeleteAllowed))
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
		if (!FileIO::IsAncestorOf(ide.GetSrcDirectory(), directory.filepath))
			directoryFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (directory.subDirectories.empty() && directory.files.empty())
			directoryFlags |= ImGuiTreeNodeFlags_Leaf;
		if (m_SelectedFilepath == directory.filepath)
			directoryFlags |= ImGuiTreeNodeFlags_Selected;

		bool opened = ImGui::TreeNodeEx(directory.filename.c_str(), directoryFlags);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			m_SelectedFilepath = directory.filepath;
			m_SelectedFilepathIsDirectory = true;
			m_SelectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.GetSrcDirectory(), m_SelectedFilepath);
			clickedFile = true;
		}

		if (!ide.IsAnyPopupOpen() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			m_SelectedFilepath = directory.filepath;
			m_SelectedFilepathIsDirectory = true;
			m_SelectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.GetSrcDirectory(), m_SelectedFilepath);
			clickedFile = true;

			m_OptionsPopupOpen = true;
		}

		if (opened)
		{
			for (auto& subDirectory : directory.subDirectories)
				RenderDirectory(subDirectory, clickedFile);

			for (auto& file : directory.files)
			{
				ImGuiTreeNodeFlags fileFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				if (m_SelectedFilepath == file.filepath)
					fileFlags |= ImGuiTreeNodeFlags_Selected;

				opened = ImGui::TreeNodeEx(file.filename.c_str(), fileFlags);

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					m_SelectedFilepath = file.filepath;
					m_SelectedFilepathIsDirectory = false;
					m_SelectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.GetSrcDirectory(), m_SelectedFilepath);
					clickedFile = true;

					if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						ide.FocusOrAddFilePanel(file.filepath);
				}

				if (!ide.IsAnyPopupOpen() && ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					m_SelectedFilepath = file.filepath;
					m_SelectedFilepathIsDirectory = false;
					m_SelectedFilepathDeleteAllowed = FileIO::IsAncestorOf(ide.GetSrcDirectory(), m_SelectedFilepath);
					clickedFile = true;

					m_OptionsPopupOpen = true;
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
			m_WorkspaceDirectory.filepath.clear();
			m_WorkspaceDirectory.filename.clear();
			m_WorkspaceDirectory.subDirectories.clear();
			m_WorkspaceDirectory.files.clear();
			m_SelectedFilepath.clear();
		}
		else
		{
			m_SelectedFilepath = workspaceDirectoryFilepath;
			m_WorkspaceDirectory.filepath = workspaceDirectoryFilepath;
			m_WorkspaceDirectory.filename = workspaceDirectoryFilepath.filename().string();
			RefreshDirectories();

			m_Notifier = DirectoryChange::Notifier(
				GBC_BIND_FUNC(OnDirectoryNotification),
				m_WorkspaceDirectory.filepath,
				DirectoryChange::NotificationType_NameChanged,
				true
			);
		}
	}

	std::filesystem::path ExplorerPanel::GetNewFileDirectory() const
	{
		auto& ide = GetIDE();

		if (!FileIO::IsAncestorOf(ide.GetSrcDirectory(), m_SelectedFilepath))
			return ide.GetSrcDirectory();
		return m_SelectedFilepathIsDirectory ? m_SelectedFilepath : m_SelectedFilepath.parent_path();
	}

	void ExplorerPanel::RefreshDirectories()
	{
		auto& ide = GetIDE();

		m_WorkspaceDirectory.subDirectories.clear();
		m_WorkspaceDirectory.files.clear();
		RefreshDirectory(m_WorkspaceDirectory);

		if (!m_SelectedFilepath.empty())
		{
			while (!FileIO::DirectoryExists(m_SelectedFilepath) && FileIO::IsAncestorOf(m_WorkspaceDirectory.filepath, m_SelectedFilepath))
				m_SelectedFilepath = m_SelectedFilepath.parent_path();
			if (!FileIO::IsAncestorOf(m_WorkspaceDirectory.filepath, m_SelectedFilepath))
				m_SelectedFilepath = m_WorkspaceDirectory.filepath;
		}
		if (m_OptionsPopupOpen)
		{
			if (!FileIO::DirectoryExists(m_SelectedFilepath))
			{
				m_SelectedFilepath.clear();
				ide.ClosePopup(s_pOptionsPopupTitle);
				m_OptionsPopupOpen = false;
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

		m_RefreshDirectories = true;
		return true;
	}
}
