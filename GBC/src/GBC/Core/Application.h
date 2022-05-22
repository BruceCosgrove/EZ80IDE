#pragma once

#include "GBC/Core/LayerStack.h"
#include "GBC/Core/Window.h"
#include "GBC/Events/WindowEvents.h"
#include "GBC/ImGui/ImGuiWrapper.h"

int main(int argc, char** argv);

namespace gbc
{
	struct CommandLineArgs
	{
		int count = 0;
		char** args = nullptr;

		constexpr const char* operator[](int index) const
		{
			GBC_CORE_ASSERT(0 <= index && index < count, "Command Line Args index out of bounds!");
			return args[index];
		}
	};

	class Application
	{
	public:
		Application(CommandLineArgs args, const WindowSpecifications& windowSpecs = {});
		virtual ~Application();

		inline static Application& Get() { return *instance; }
		inline Window& GetWindow() { return *window; }
		inline void* GetFocusedNativeWindow() { return focusedNativeWindow; }
#if GBC_ENABLE_IMGUI
		inline ImGuiWrapper& GetImGuiWrapper() { return *imguiWrapper; }
#endif

		void Close();
	public:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		Layer* PopLayer();
		Layer* PopOverlay();
	private:
		void OnEvent(Event& event);
		void OnWindowCloseEvent(WindowCloseEvent& event);
		void OnWindowFramebufferResizeEvent(WindowFramebufferResizeEvent& event);
		void OnWindowFocusEvent(WindowFocusEvent& event);
		void OnWindowMinimizeEvent(WindowMinimizeEvent& event);
	private:
		static Application* instance;
		Scope<Window> window;
		void* focusedNativeWindow = nullptr;
#if GBC_ENABLE_IMGUI
		ImGuiWrapper* imguiWrapper;
#endif
		LayerStack layerStack;
	private:
		bool running = true;
		bool rendering = true;
		bool windowZeroSize = false;
	private:
		friend int ::main(int argc, char** argv);
		void Run();
	};

	Application* CreateApplication(CommandLineArgs args);
}
