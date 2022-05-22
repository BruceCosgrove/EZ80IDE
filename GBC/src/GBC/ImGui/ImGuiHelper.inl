namespace gbc
{
	template<typename... Args>
	static void ImGuiHelper::Text(const char* label, const char* format, Args&&... args)
	{
		ImGui::PushID(label);

		NextTableColumn();
		ImGui::Text(format, std::forward<Args>(args)...);
		PrevTableColumn();
		ImGui::Text(label);
		NextTableColumn();

		ImGui::PopID();
	}

	template<size_t Size>
	static bool ImGuiHelper::InputText(char(&buffer)[Size], ImGuiInputTextFlags flags)
	{
		return ImGui::InputText("", buffer, Size, flags);
	}

	template<typename... Args>
	static bool ImGuiHelper::ConfirmAction(const char* name, bool* action, const char* messageFormat, Args&&... args)
	{
		bool closed = false;

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::OpenPopup(name);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2{ 0.5f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.0f });
		if (ImGui::BeginPopupModal(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text(messageFormat, std::forward<Args>(args)...);

			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { style.ItemSpacing.x * 0.5f, style.ItemSpacing.y });
			float availableWidth = ImGui::GetContentRegionAvail().x;
			float padding = style.FramePadding.x;
			auto width = (availableWidth - padding) * 0.5f;
			ImVec2 buttonSize = { width, 0.0f };

			if (ImGui::Button("Yes", buttonSize))
			{
				*action = true;
				closed = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("No", buttonSize))
			{
				*action = false;
				closed = true;
			}

			ImGui::PopStyleVar();

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar(2);

		return closed;
	}

	template<typename... Args>
	static bool ImGuiHelper::Inform(const char* name, const char* messageFormat, Args&&... args)
	{
		bool closed = false;

		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f });
		ImGui::OpenPopup(name);

		if (ImGui::BeginPopupModal(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::Text(messageFormat, std::forward<Args>(args)...);

			ImGui::PushItemWidth(-FLT_MIN);
			if (ImGui::Button("OK", { ImGui::GetContentRegionAvail().x, 0.0f }))
				closed = true;
			ImGui::PopItemWidth();

			ImGui::EndPopup();
		}

		return closed;
	}
}
