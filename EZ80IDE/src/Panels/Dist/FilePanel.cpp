#include "FilePanel.h"
#if GBC_ENABLE_IMGUI
#include "Layers/EZ80IDELayer.h"
#include "GBC/ImGui/ImGuiHelper.h"
#include <imgui/imgui_internal.h>
#include <unordered_map>

namespace gbc
{
	FilePanel::FilePanel(EZ80IDELayer* ez80IDELayer, const std::filesystem::path& filepath, uint64_t id)
		: Panel(ez80IDELayer, ez80IDELayer->GetFilePanelTitle(filepath, id), false), filepath(filepath)
	{
		bool status = FileIO::MakeFileIfNotExists(filepath);
		GBC_ASSERT(status, "Failed to create file panel file!");
		UpdateWindowTitle();
		SetEnabled(true);
	}

	FilePanel::~FilePanel()
	{
		auto& ide = GetIDE();
		ide.RemoveFilePanel(filepath);

		Save();
		SetEnabled(false);
	}

	void FilePanel::OnImGuiRender(int)
	{
		// TODO: text editor
	}

	void FilePanel::UpdateWindowTitle()
	{
		windowTitle = filepath.filename().string();
		asteriskIndex = windowTitle.length();
		auto& defaultTitle = GetDefaultTitle();
		windowTitle.reserve(asteriskIndex + 1 + defaultTitle.length());
		windowTitle += unsavedChanges ? '*' : ' ';
		windowTitle += defaultTitle;
	}

	void FilePanel::SetFilepath(const std::filesystem::path& filepath) noexcept
	{
		auto& ide = GetIDE();
		ide.RenameFilePanel(this->filepath, filepath);
		this->filepath = filepath;
	}

	void FilePanel::SetEnabled(bool enabled) noexcept
	{
		Panel::SetEnabled(enabled);
		if (HasEnabledChanged())
		{
			if (enabled)
			{
				// If the window is being reopened, reallow it to save its state.
				if (ImGuiWindow* window = ImGui::FindWindowByName(windowTitle.c_str()))
					window->Flags &= ~ImGuiWindowFlags_NoSavedSettings;
			}
			else
			{
				// Prevent the window from saving its state to new workspace ini files to avoid cross-workspace contamination.
				if (ImGuiWindow* window = ImGui::FindWindowByName(windowTitle.c_str()))
					window->Flags |= ImGuiWindowFlags_NoSavedSettings;
			}
		}
	}

	void FilePanel::SetFocused(bool focused) noexcept
	{
		Panel::SetFocused(focused);
		if (HasFocusChanged() && focused)
			ImGui::SetWindowFocus(windowTitle.c_str());
	}

	void FilePanel::OnChanged()
	{
		if (!unsavedChanges)
		{
			unsavedChanges = true;
			windowTitle[asteriskIndex] = '*';
		}
	}

	void FilePanel::Save()
	{
		if (unsavedChanges)
		{
			unsavedChanges = false;
			windowTitle[asteriskIndex] = ' ';

			//std::ofstream file(filepath);
			//if (file.is_open())
			//{
			//	// TODO: write data

			//	file.close();
			//}
		}
	}
}
#endif
