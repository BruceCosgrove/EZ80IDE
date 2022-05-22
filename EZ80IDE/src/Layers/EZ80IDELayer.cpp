#include "EZ80IDELayer.h"
#include "GBC/Util/Util.h"
#include "Panels/Dist/ExplorerPanel.h"
#include "Panels/Dist/FilePanel.h"
#include "Panels/Release/RendererInfoPanel.h"
#include "Panels/Debug/ProfilingPanel.h"
#include "Panels/Debug/DemoPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <fstream>

namespace gbc
{
	void EZ80IDELayer::OnAttach()
	{
		GBC_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		constexpr char defaultImGuiIniData[] = {
R"(
[Window][Dockspace]
Pos=0,0
Size=1920,1017
Collapsed=0

[Window][Explorer]
Pos=0,26
Size=329,991
Collapsed=0
DockId=0x00000001,0

[Docking][Data]
DockSpace   ID=0x33675C32 Window=0x5B816B74 Pos=0,49 Size=1920,991 Split=X
  DockNode  ID=0x00000001 Parent=0x33675C32 SizeRef=329,874 Selected=0x1E89CEB8
  DockNode  ID=0x00000002 Parent=0x33675C32 SizeRef=1269,874 CentralNode=1
)"
		};
		ImGui::LoadIniSettingsFromMemory(defaultImGuiIniData, sizeof(defaultImGuiIniData));

#if GBC_CONFIG_DEBUG
		AddPanel<DemoPanel>("Demo")->SetEnabled(false);
#endif
#if GBC_ENABLE_PROFILE_RUNTIME
		AddPanel<ProfilingPanel>("Profiling")->SetEnabled(false);
#endif
#if GBC_ENABLE_STATS
		AddPanel<RendererInfoPanel>("Renderer Info")->SetEnabled(false);
#endif

		explorerPanel = AddPanel<ExplorerPanel>("Explorer");
	}

	void EZ80IDELayer::OnDetach()
	{
		GBC_PROFILE_FUNCTION();

		SaveAndCloseWorkspace();
	}

	void EZ80IDELayer::OnUpdate(Timestep timestep)
	{
		GBC_PROFILE_FUNCTION();

		//bool viewportFocused = sceneViewportPanel->IsFocused();
		//bool viewportHovered = sceneViewportPanel->IsHovered();

		// TODO: Set this depending on state.
		//Application::Get().GetImGuiWrapper().SetBlockEvents(!sceneViewportPanel->IsFocused() && !sceneViewportPanel->IsHovered());

		panels.UpdateOrder();
		SetStates(IDEState_ExplorerFocused, explorerPanel->IsFocused());

		filePanels.UpdateOrder();
		SetStates(IDEState_FileFocused, filePanels.HasFocusedPanel());
	}

	void EZ80IDELayer::OnRender()
	{
		GBC_PROFILE_FUNCTION();

		Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		Renderer::Clear();
	}

	void EZ80IDELayer::OnImGuiRender()
	{
		GBC_PROFILE_FUNCTION();

		UI_Dockspace();

		for (auto panel : panels)
			panel->OnImGuiRender();
		for (auto filePanel : filePanels)
			filePanel->OnImGuiRender();

		if (openPopup)
			(this->*openPopup)();

		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(2360, 0), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Panel Debug", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
#define DEBUG_DRAW_PANEL_STACK(name, stack) \
			ImGui::Text(name); \
			ImGuiHelper::BeginTable(name, 3, ImGuiHelper::defaultTableFlags | ImGuiTableFlags_Borders); \
			for (auto it = stack.crbegin(); it != stack.crend(); ++it) \
			{ \
				Panel* panel = *it; \
				ImGui::Text(panel->IsEnabled() ? "Enabled" : "Disabled"); \
				ImGuiHelper::NextTableColumn(); \
				ImGuiHelper::Text(panel->IsFocused() ? "Focused" : "Unfocused", panel->GetTitle().c_str()); \
				if (it != stack.crend() - 1) \
					ImGuiHelper::NextTableColumn(); \
			} \
			ImGuiHelper::EndTable();

			DEBUG_DRAW_PANEL_STACK("File Panels", filePanels);
			ImGui::NewLine();
			DEBUG_DRAW_PANEL_STACK("Other Panels", panels);
#undef DEBUG_DRAW_PANEL_STACK
			ImGui::End();
		}

		ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(2760, 0), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("State Debug", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
#define DEBUG_DRAW_STATE(x) ImGui::Text(#x "=%d", !!(state & IDEState_##x))
			ImGui::Text("Generic");
			ImGui::Indent();
			DEBUG_DRAW_STATE(WorkspaceOpen);
			DEBUG_DRAW_STATE(FileOpen);
			DEBUG_DRAW_STATE(FileFocused);
			DEBUG_DRAW_STATE(ExplorerFocused);
			DEBUG_DRAW_STATE(PopupOpen);
			DEBUG_DRAW_STATE(ModalPopup);
			ImGui::Unindent();
#undef DEBUG_DRAW_STATE
			ImGui::End();
		}
	}

	void EZ80IDELayer::OnPostImGuiRender()
	{

	}

	void EZ80IDELayer::UI_Dockspace()
	{
		ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f });
		ImGui::Begin("Dockspace", nullptr, dockspaceFlags);
		ImGui::PopStyleVar(2);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 200.0f, ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f });
		ImGui::DockSpace(ImGui::GetID("Dockspace"));
		ImGui::PopStyleVar();

		UI_MenuBar();
		ImGui::End();
	}

	void EZ80IDELayer::UI_MenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open Workspace...", "Ctrl+O", false, HasSubStates(IDEState_None, IDEState_ModalPopup)))
					OpenWorkspace();
				if (ImGui::MenuItem("New File...", "Ctrl+N", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					NewFile();
				if (ImGui::MenuItem("New Folder...", "Ctrl+Shift+N", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					NewFolder();
				if (ImGui::MenuItem("Save File", "Ctrl+S", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					SaveFile();
				if (ImGui::MenuItem("Save All", "Ctrl+Shift+S", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					SaveAll();
				if (ImGui::MenuItem("Rename...", "Ctrl+R", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					Rename();
				if (ImGui::MenuItem("Close File", "Ctrl+W", false, HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup)))
					CloseFile();
				if (ImGui::MenuItem("Close Workspace", "Ctrl+Shift+W", false, HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup)))
					CloseWorkspace();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit", "Alt+F4"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::BeginMenu("Panels"))
				{
					for (auto panel : panels)
						if (ImGui::MenuItem(panel->GetTitle().c_str(), nullptr, panel->IsEnabled()))
							panel->ToggleEnabled();
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EZ80IDELayer::UI_Popup_NewFile()
	{
		static constexpr char title[] = "New File";
		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static constexpr const char* extensions[]{ ".ez80", ".inc", ".asm" };
		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool create = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (ImGuiHelper::BeginTable("NewFilePopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				ImGuiHelper::Text("Directory", newFileInitialDirectoryString.c_str());
				ImGuiHelper::NextTableColumn();

				(void)ImGuiHelper::InputText("Name", newFilenameBuffer, sizeof(newFilenameBuffer) / sizeof(*newFilenameBuffer));
				ImGuiHelper::NextTableColumn();

				(void)ImGuiHelper::Combo("Extension", &newFilePendingExtension, extensions, sizeof(extensions) / sizeof(*extensions));

				ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Create", buttonSize) || Input::IsKeyPressed(Keycode::Enter))
			{
				closed = true;
				create = !!*newFilenameBuffer;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || Input::IsKeyPressed(Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				openPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (create)
			{
				newFileSelectedExtension = newFilePendingExtension;

				std::filesystem::path filepath = workspaceDirectory;
				filepath /= newFileInitialDirectory;
				filepath /= (char*)newFilenameBuffer;
				filepath.replace_extension(extensions[newFileSelectedExtension]);
				FocusOrAddFilePanel(filepath);
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_NewFolder()
	{
		static constexpr char title[] = "New Folder";
		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool create = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (ImGuiHelper::BeginTable("NewFolderPopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				ImGuiHelper::Text("Directory", newFileInitialDirectoryString.c_str());
				ImGuiHelper::NextTableColumn();

				(void)ImGuiHelper::InputText("Name", newFilenameBuffer, sizeof(newFilenameBuffer) / sizeof(*newFilenameBuffer));

				ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Create", buttonSize) || Input::IsKeyPressed(Keycode::Enter))
			{
				closed = true;
				create = !!*newFilenameBuffer;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || Input::IsKeyPressed(Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				openPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (create)
			{
				std::filesystem::path filepath = workspaceDirectory;
				filepath /= newFileInitialDirectory;
				filepath /= (char*)newFilenameBuffer;
				bool status = FileIO::MakeDirectoryIfNotExists(filepath);
				GBC_ASSERT(status, "Failed to create new directory.");
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_Rename()
	{
		static constexpr char titleFile[] = "Rename File";
		static constexpr char titleFolder[] = "Rename Folder";
		const char* title = renamingFolder ? titleFolder : titleFile;

		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		static constexpr const char* extensions[]{ ".ez80", ".inc", ".asm" };
		static bool firstFrame = true;
		static bool secondFrame = false;
		bool closed = false;
		bool rename = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			if (ImGuiHelper::BeginTable("RenamePopupTable", 2, ImGuiTableFlags_None))
			{
				if (secondFrame)
					ImGui::SetKeyboardFocusHere();
				ImGuiHelper::Text("Directory", newFileInitialDirectoryString.c_str());
				ImGuiHelper::NextTableColumn();

				(void)ImGuiHelper::InputText("Name", newFilenameBuffer, sizeof(newFilenameBuffer) / sizeof(*newFilenameBuffer));

				if (!renamingFolder)
				{
					ImGuiHelper::NextTableColumn();
					(void)ImGuiHelper::Combo("Extension", &newFilePendingExtension, extensions, sizeof(extensions) / sizeof(*extensions));
				}

				ImGuiHelper::EndTable();
			}

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Rename", buttonSize) || Input::IsKeyPressed(Keycode::Enter))
			{
				closed = true;
				rename = !!*newFilenameBuffer; // rename if newFilenameBuffer is not empty
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || Input::IsKeyPressed(Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				openPopup = nullptr;
				firstFrame = true;
				secondFrame = false;
			}
			else if (firstFrame)
			{
				firstFrame = false;
				secondFrame = true;
			}
			else if (secondFrame)
				secondFrame = false;

			if (rename)
			{
				auto filepathNew = workspaceDirectory / newFileInitialDirectory;
				filepathNew /= (char*)newFilenameBuffer;

				if (renamingFolder)
				{
					auto filepathOld = workspaceDirectory / newFileInitialDirectory;
					bool status = FileIO::RenameDirectory(filepathOld, filepathNew);
					GBC_ASSERT(status, "Failed to rename directory.");

					for (auto panel : filePanels)
					{
						FilePanel* filePanel = (FilePanel*)panel;
						if (FileIO::IsAncestorOf(filepathOld, filePanel->GetFilepath()))
						{
							auto relative = FileIO::Relative(filePanel->GetFilepath(), filepathOld);
							RenameFilePanel(filePanel->GetFilepath(), filepathNew / relative);
						}
					}
				}
				else
				{
					newFileSelectedExtension = newFilePendingExtension;
					filepathNew.replace_extension(extensions[newFileSelectedExtension]);

					if (renamingOpenFile)
						RenameFilePanel(((FilePanel*)filePanels.Peek())->GetFilepath(), filepathNew);
					else
					{
						bool status = FileIO::RenameFile(explorerPanel->GetSelectedFilepath(), filepathNew);
						GBC_ASSERT(status, "Failed to rename file.");
					}
				}
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::UI_Popup_ConfirmDelete()
	{
		static constexpr char titleFile[] = "Delete File";
		static constexpr char titleFolder[] = "Delete Folder";
		const char* title = deletingFolder ? titleFolder : titleFile;

		static constexpr ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup;
		static constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

		ImGui::OpenPopup(title, popupFlags);

		bool closed = false;
		bool delet3 = false;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::SetNextWindowSize(ImVec2{ 300.0f, 0.0f }, ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f, ImGui::GetStyle().WindowTitleAlign.y });
		if (ImGui::BeginPopupModal(title, nullptr, windowFlags))
		{
			ImGui::Text("Are you sure you want to delete:\n%s?", deletingMessage.c_str());

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			ImVec2 buttonSize{ (ImGui::GetContentRegionAvail().x - style.FramePadding.x) * 0.5f, 0.0f };

			if (ImGui::Button("Delete", buttonSize) || Input::IsKeyPressed(Keycode::Enter))
			{
				closed = true;
				delet3 = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", buttonSize) || Input::IsKeyPressed(Keycode::Escape))
				closed = true;

			ImGui::PopStyleVar();

			if (closed)
			{
				ImGui::CloseCurrentPopup();
				RemoveStates(IDEState_PopupOpen | IDEState_ModalPopup);
				openPopup = nullptr;
			}

			if (delet3)
			{
				if (explorerPanel->IsSelectedFilepathADirectory())
					DeleteFolder(explorerPanel->GetSelectedFilepath());
				else
					DeleteFilePanel(explorerPanel->GetSelectedFilepath());
			}

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}

	void EZ80IDELayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch(this, &EZ80IDELayer::OnWindowCloseEvent);
		dispatcher.Dispatch(this, &EZ80IDELayer::OnKeyPressEvent);
		if (!filePanels.HasFocusedPanel())
			dispatcher.Dispatch(&panels, &PanelStack::OnEvent);
		if (!panels.HasFocusedPanel())
			dispatcher.Dispatch(&filePanels, &PanelStack::OnEvent);
	}

	void EZ80IDELayer::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		if (IsState(IDEState_WorkspaceOpen))
			CloseWorkspace();
	}

	void EZ80IDELayer::OnKeyPressEvent(KeyPressEvent& event)
	{
		Keycode keycode = event.GetKeycode();
		Mods mods = event.GetMods();

		if (HasSubStates(IDEState_WorkspaceOpen | IDEState_FileOpen, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case Keycode::S:
				if (mods.Are(Mods_Control))
					SaveFile();
				else if (mods.Are(Mods_Control | Mods_Shift))
					SaveAll();
				break;
			case Keycode::W:
				if (mods.Are(Mods_Control))
					CloseFile();
				break;
			}
		}

		if (HasSubStates(IDEState_WorkspaceOpen | IDEState_ExplorerFocused))
		{
			switch (keycode)
			{
			case Keycode::Delete:
				if (mods.Are(Mods_None) && explorerPanel->IsSelectedFilepathDeleteAllowed())
					Delete();
			case Keycode::C:
				if (mods.Are(Mods_Shift))
					CopyPath(explorerPanel->GetSelectedFilepath());
				if (mods.Are(Mods_Shift | Mods_Alt))
					CopyRelativePath(explorerPanel->GetSelectedFilepath());
				break;
			}
		}

		if (HasSubStates(IDEState_WorkspaceOpen, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case Keycode::N:
				if (mods.Are(Mods_Control))
					NewFile();
				else if (mods.Are(Mods_Control | Mods_Shift))
					NewFolder();
				break;
			case Keycode::R:
				if (mods.Are(Mods_Control))
					Rename();
				break;
			case Keycode::W:
				if (mods.Are(Mods_Control | Mods_Shift))
					CloseWorkspace();
				break;
			}
		}

		if (HasSubStates(IDEState_None, IDEState_ModalPopup))
		{
			switch (keycode)
			{
			case Keycode::O:
				if (mods.Are(Mods_Control))
					OpenWorkspace();
				break;
			}
		}
	}

	const std::string& EZ80IDELayer::GetFilePanelTitle(const std::filesystem::path& filepath, uint64_t id)
	{
		auto it = titleIDs.find(filepath);
		if (it != titleIDs.end())
			return it->second;

		// Get the true id.
		uint64_t titleID;
		if (id)
		{
			titleID = id;
			if (id > nextTitleID)
				nextTitleID = id;
		}
		else
			titleID = ++nextTitleID;

		auto& titleIDString = titleIDs[filepath];
		util::IToS(titleID, titleIDString, 36);
		titleIDString.insert(0, FilePanel::identifier);
		return titleIDString;
	}

	void EZ80IDELayer::RenameFilePanel(const std::filesystem::path& filepathOld, const std::filesystem::path& filepathNew)
	{
		auto it = titleIDs.find(filepathOld);
		if (it != titleIDs.end())
		{
			// Remove the previous entry before adding the new
			// one because that could invalidate the former.
			std::string titleIDString = std::move(it->second);
			titleIDs.erase(it);
			titleIDs[filepathNew] = std::move(titleIDString);

			for (auto panel : filePanels)
			{
				FilePanel* filePanel = (FilePanel*)panel;
				if (filePanel->GetFilepath() == filepathOld)
				{
					filePanel->Rename(filepathNew);
					break;
				}
			}
		}
	}

	void EZ80IDELayer::RemoveFilePanel(const std::filesystem::path& filepath)
	{
		auto it = titleIDs.find(filepath);
		if (it != titleIDs.end())
			titleIDs.erase(it);
	}

	void EZ80IDELayer::DeleteFilePanel(const std::filesystem::path& filepath)
	{
		if (FileIO::IsAncestorOf(srcDirectory, filepath) && FileIO::FileExists(filepath))
		{
			auto it = titleIDs.find(filepath);
			if (it != titleIDs.end())
			{
				titleIDs.erase(it);

				for (auto it = filePanels.begin(); it != filePanels.end(); ++it)
				{
					FilePanel* filePanel = (FilePanel*)(*it);
					if (filePanel->GetFilepath() == filepath)
					{
						filePanels.Remove(it - filePanels.begin());
						break;
					}
				}
			}

			FileIO::Delete(filepath);
		}
	}

	void EZ80IDELayer::DeleteFolder(const std::filesystem::path& filepath)
	{
		if (FileIO::IsAncestorOf(srcDirectory, filepath) && FileIO::DirectoryExists(filepath))
		{
			if (FileIO::IsEmptyDirectory(filepath))
				FileIO::Delete(filepath);
			else
			{
				for (size_t i = 0; i < filePanels.Size();)
				{
					FilePanel* filePanel = (FilePanel*)filePanels[i];
					auto& filePanelFilepath = filePanel->GetFilepath();
					if (FileIO::IsAncestorOf(filepath, filePanelFilepath))
					{
						titleIDs.erase(filePanelFilepath);
						filePanels.Remove(i);
					}
					else
						i++;
				}

				FileIO::DeleteRecursive(filepath);
			}
		}
	}

	bool EZ80IDELayer::FocusFilePanel(const std::filesystem::path& filepath)
	{
		auto it = titleIDs.find(filepath);
		if (it == titleIDs.end())
			return false;

		for (auto it = filePanels.begin(); it != filePanels.end(); ++it)
		{
			FilePanel* filePanel = (FilePanel*)(*it);
			if (filePanel->GetFilepath() == filepath)
			{
				filePanels.Focus(it - filePanels.begin());
				if (filePanel->HasEnabledChanged())
					AddStates(IDEState_FileOpen);
				return true;
			}
		}

		// Shouldn't happen.
		GBC_ASSERT(false, "File panel not found in file panel stack, but found in title id registry.");
		return false;
	}

	void EZ80IDELayer::FocusOrAddFilePanel(const std::filesystem::path& filepath)
	{
		if (!FocusFilePanel(filepath))
			(void)AddFilePanel(filepath);
	}

	bool EZ80IDELayer::OpenPopup(const char* name, ImGuiPopupFlags flags)
	{
		if (!IsAnyPopupOpen())
		{
			AddStates(IDEState_PopupOpen);
			ImGui::OpenPopup(name, flags | ImGuiPopupFlags_NoOpenOverExistingPopup);
			return true;
		}

		if (ImGui::IsPopupOpen(name))
			return true;

		RemoveStates(IDEState_PopupOpen);
		return false;
	}

	bool EZ80IDELayer::IsAnyPopupOpen()
	{
		return HasSubStates(IDEState_PopupOpen);
	}

	bool EZ80IDELayer::IsPopupOpen(const char* name, ImGuiPopupFlags flags)
	{
		return IsAnyPopupOpen() && ImGui::IsPopupOpen(name, flags);
	}

	void EZ80IDELayer::ClosePopup(const char* name)
	{
		if (IsAnyPopupOpen())
		{
			RemoveStates(IDEState_PopupOpen);
			if (ImGui::IsPopupOpen(name))
				ImGui::CloseCurrentPopup();
		}
	}

	FilePanel* EZ80IDELayer::AddFilePanel(const std::filesystem::path& filepath, uint64_t id)
	{
		AddStates(IDEState_FileOpen);

		// TODO: set position, size, docking
		FilePanel* panel = new FilePanel(this, filepath, id);
		filePanels.Push(panel);
		return panel;
	}

	void EZ80IDELayer::NewFile()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("New File");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			openPopup = &EZ80IDELayer::UI_Popup_NewFile;

			newFilePendingExtension = newFileSelectedExtension;
			memset(&newFilenameBuffer, 0, sizeof(newFilenameBuffer));

			newFileInitialDirectory = FileIO::Relative(explorerPanel->GetNewFileDirectory(), workspaceDirectory);
			newFileInitialDirectoryString = newFileInitialDirectory.string();
			newFileInitialDirectoryString.insert(0, 1, '.');
			newFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			newFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::NewFolder()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("New Folder");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			openPopup = &EZ80IDELayer::UI_Popup_NewFolder;

			newFilePendingExtension = newFileSelectedExtension;
			memset(&newFilenameBuffer, 0, sizeof(newFilenameBuffer));

			newFileInitialDirectory = FileIO::Relative(explorerPanel->GetNewFileDirectory(), workspaceDirectory);
			newFileInitialDirectoryString = newFileInitialDirectory.string();
			newFileInitialDirectoryString.insert(0, 1, '.');
			newFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			newFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::OpenWorkspace()
	{
		GBC_INFO("Open Workspace");

		auto filepath = FileDialog::OpenFolder();
		if (!filepath.empty() && FileIO::DirectoryExists(filepath))
		{
			if (HasSubStates(IDEState_WorkspaceOpen))
				CloseWorkspace();
			SetWorkspaceDirectory(filepath);
		}
	}

	void EZ80IDELayer::SaveFile()
	{
		GBC_INFO("Save File");

		((FilePanel*)filePanels.Peek())->Save();
	}

	void EZ80IDELayer::SaveAll()
	{
		GBC_INFO("Save All");

		for (auto panel : filePanels)
			if (panel->IsEnabled())
				((FilePanel*)panel)->Save();
	}

	void EZ80IDELayer::Rename()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			std::filesystem::path filename;
			std::wstring_view extension;
			if (explorerPanel->IsFocused())
			{
				filename = explorerPanel->GetSelectedFilepath().filename();
				extension = std::filesystem::_Parse_extension(filename.native());
				newFileInitialDirectory = FileIO::Relative(explorerPanel->GetNewFileDirectory(), workspaceDirectory);
				renamingFolder = extension.empty();
				if (renamingFolder)
					newFileInitialDirectory = newFileInitialDirectory.parent_path();
				else if (!filePanels.Empty())
				{
					renamingOpenFile = false;
					for (auto panel : filePanels)
					{
						FilePanel* filePanel = (FilePanel*)panel;
						if (filePanel->GetFilepath() == explorerPanel->GetSelectedFilepath())
						{
							renamingOpenFile = true;
							break;
						}
					}
				}
			}
			else if (HasSubStates(IDEState_FileOpen))
			{
				renamingOpenFile = true;
				renamingFolder = false;

				FilePanel* filePanel = (FilePanel*)filePanels.Peek();
				filename = filePanel->GetFilepath().filename();
				extension = std::filesystem::_Parse_extension(filename.native());
				newFileInitialDirectory = FileIO::Relative(filePanel->GetFilepath(), workspaceDirectory).parent_path();
			}
			else // Nothing to rename.
				return;

			GBC_INFO("Rename");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			openPopup = &EZ80IDELayer::UI_Popup_Rename;

			std::string filenameString;
			if (renamingFolder)
				filenameString = filename.string();
			else
			{
				filenameString = filename.replace_extension().string();
				extension = extension.substr(1);

				newFilePendingExtension = 0;
				if (extension == L"inc")
					newFilePendingExtension = 1;
				else if (extension == L"asm")
					newFilePendingExtension = 2;
			}

			strcpy_s(newFilenameBuffer, filenameString.length() + 1, filenameString.c_str());

			newFileInitialDirectoryString = newFileInitialDirectory.string();
			newFileInitialDirectoryString.insert(0, 1, '.');
			newFileInitialDirectoryString.insert(1, 1, static_cast<char>(std::filesystem::path::preferred_separator));
			newFileInitialDirectoryString += static_cast<char>(std::filesystem::path::preferred_separator);
		}
	}

	void EZ80IDELayer::Delete()
	{
		if (!HasAnySubStates(IDEState_ModalPopup))
		{
			GBC_INFO("Delete");

			AddStates(IDEState_PopupOpen | IDEState_ModalPopup);
			openPopup = &EZ80IDELayer::UI_Popup_ConfirmDelete;

			deletingFolder = explorerPanel->IsSelectedFilepathADirectory();
			deletingMessage = explorerPanel->GetSelectedFilepath().filename().string();
			if (deletingFolder && !FileIO::IsEmptyDirectory(explorerPanel->GetSelectedFilepath()))
				deletingMessage += " and all its contents";
		}
	}

	void EZ80IDELayer::CloseFile()
	{
		GBC_INFO("Close File");

		// TODO: close with or without saving dialog
		// check box for save my answer, use it every time this happens, and do not show this again + change later in settings
		//((FilePanel*)filePanels.Peek())->MarkForCloseWithoutSaving();

		filePanels.Close();

		if (filePanels.GetOpenPanelCount() <= 1) // off by one
			RemoveStates(IDEState_FileOpen);
	}

	void EZ80IDELayer::CloseWorkspace()
	{
		GBC_INFO("Close Folder");

		// TODO: close with or without saving dialog
		// check box for save my answer, use it every time this happens, and do not show this again + change later in settings

		SetWorkspaceDirectory({});
	}

	void EZ80IDELayer::CopyPath(const std::filesystem::path& filepath)
	{
		Input::SetClipboardString(filepath.string());
	}

	void EZ80IDELayer::CopyRelativePath(const std::filesystem::path& filepath)
	{
		Input::SetClipboardString(FileIO::Relative(filepath, workspaceDirectory).string());
	}

	void EZ80IDELayer::SaveAndCloseWorkspace()
	{
		if (HasSubStates(IDEState_WorkspaceOpen))
		{
			// Remove all closed panels so they aren't written to the title ids file.
			filePanels.RemoveClosedPanels();

			// Save title IDs.
			std::ofstream titleIDsFile(titleIDsFilepath);
			if (titleIDsFile.is_open())
			{
				for (auto& [filepath, id] : titleIDs)
				{
					auto filepathString = filepath.string();
					titleIDsFile.write(filepathString.c_str(), filepathString.length());
					titleIDsFile.put('\n');
					titleIDsFile.write(id.c_str() + FilePanel::identifierLength, id.length() - FilePanel::identifierLength);
					titleIDsFile.put('\n');
				}

				titleIDs.clear();
				nextTitleID = 0;

				titleIDsFile.close();
			}
			else
				GBC_ASSERT(false, "Couldn't save title ids!");

			// Save the old workspace imgui ini.
			ImGui::SaveIniSettingsToDisk(imguiIniFilepathString.c_str());

			// Save all unsaved files, delete all file panels,
			// and make all file imgui windows not persist to next workspace ini.
			filePanels.Clear();
		}
	}

	void EZ80IDELayer::SetWorkspaceDirectory(const std::filesystem::path& workspaceDirectory)
	{
		SaveAndCloseWorkspace();

		if (workspaceDirectory.empty())
		{
			SetState(IDEState_None);

			this->workspaceDirectory.clear();
			metaDataDirectory.clear();
			imguiIniFilepathString.clear();
			srcDirectory.clear();
			binDirectory.clear();
			titleIDsFilepath.clear();
		}
		else
		{
			AddStates(IDEState_WorkspaceOpen);

			// Update workspace to new directory.
			this->workspaceDirectory = FileIO::Absolute(workspaceDirectory);

			srcDirectory = workspaceDirectory / L"src";
			FileIO::MakeDirectoryIfNotExists(srcDirectory);
			binDirectory = workspaceDirectory / L"bin";
			FileIO::MakeDirectoryIfNotExists(binDirectory);

			metaDataDirectory = workspaceDirectory / L".ez80ide";
			FileIO::MakeHiddenDirectory(metaDataDirectory);

			std::filesystem::path imguiIniFilepath = metaDataDirectory / L"imgui.ini";
			imguiIniFilepathString = imguiIniFilepath.string();
			if (FileIO::FileExists(imguiIniFilepath))
				ImGui::LoadIniSettingsFromDisk(imguiIniFilepathString.c_str());

			titleIDsFilepath = metaDataDirectory / L"title_ids.ini";
			if (FileIO::FileExists(titleIDsFilepath))
			{
				std::string titleIDFile = FileIO::ReadFile(titleIDsFilepath);

				size_t filepathStart = 0;
				do
				{
					size_t filepathEnd = titleIDFile.find('\n', filepathStart);
					if (filepathEnd == std::string::npos)
						break;

					auto titleIDFileView = (std::string_view)titleIDFile;
					std::filesystem::path titleIDFilepath(titleIDFileView.substr(filepathStart, filepathEnd - filepathStart));
					size_t idBegin = filepathEnd + 1;
					size_t idEnd = titleIDFileView.find('\n', idBegin + 1);
					auto titleIDString = titleIDFileView.substr(idBegin, idEnd - idBegin);
					filepathStart = idEnd + 1;

					uint64_t titleID;
					if (!util::SToI(titleIDString, titleID, 36))
						titleID = 0;
					FilePanel* filePanel = AddFilePanel(titleIDFilepath, titleID);

					// I would move titleIDFilepath here, but std::filesystem::path has the default
					// move constructor, not anything that actually MOVEs the filepath's memory.
					titleIDs[titleIDFilepath] = filePanel->GetDefaultTitle();
				}
				while (true);
			}
		}

		explorerPanel->SetWorkspaceDirectory(this->workspaceDirectory);
	}
}
