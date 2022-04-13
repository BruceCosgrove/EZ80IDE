#include "gbcpch.h"
#include "Application.h"
#include "GBC/Core/Input.h"
#include "GBC/Core/Timestep.h"
#include "GBC/Events/WindowEvents.h"
#include "GBC/Rendering/Renderer.h"

namespace gbc
{
	Application* Application::instance = nullptr;

	Application::Application(CommandLineArgs args, const WindowSpecifications& windowSpecs)
	{
		GBC_PROFILE_FUNCTION();

		GBC_CORE_ASSERT(instance == nullptr, "Application already exists!");
		instance = this;

		auto eventCallback = GBC_BIND_FUNC(OnEvent);
		window = Window::Create(windowSpecs);
		window->SetEventCallback(eventCallback);
		Input::SetEventCallback(eventCallback);

		if (windowSpecs.focusOnShow)
		{
			WindowFocusEvent wfe(window->GetNativeWindow(), true);
			OnWindowFocusEvent(wfe);
		}

		Renderer::Init();

#if GBC_ENABLE_IMGUI
		imguiWrapper = new ImGuiWrapper();
#else
		Renderer::SetViewport(0, 0, window->GetWidth(), window->GetHeight());
#endif
	}

	Application::~Application()
	{
		GBC_PROFILE_FUNCTION();

		for (Layer* layer : layerStack)
		{
			layer->OnDetach();
			delete layer;
		}

#if GBC_ENABLE_IMGUI
		delete imguiWrapper;
#endif

		Renderer::Shutdown();
	}

	void Application::Run()
	{
		Timestep timestep;

		while (running)
		{
			GBC_PROFILE_SCOPE("RunLoop");

			timestep = window->GetContext().GetElapsedTime();

			for (Layer* layer : layerStack)
				if (layer->IsEnabled())
					layer->OnUpdate(timestep);

#if GBC_ENABLE_IMGUI
			if (imguiWrapper->ImGuiHasWindows() || rendering)
			{
				for (Layer* layer : layerStack)
					if (layer->IsEnabled())
						layer->OnRender();

				imguiWrapper->Begin();
				for (Layer* layer : layerStack)
					if (layer->IsEnabled())
						layer->OnImGuiRender();
				imguiWrapper->End();

				for (Layer* layer : layerStack)
					if (layer->IsEnabled())
						layer->OnPostImGuiRender();

				window->SwapBuffers();
			}
#else
			if (rendering)
			{
				for (Layer* layer : layerStack)
					if (layer->IsEnabled())
						layer->OnRender();

				window->SwapBuffers();
			}
#endif

			window->PollEvents();
		}
	}

	void Application::Close()
	{
		running = false;
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	Layer* Application::PopLayer()
	{
		Layer* layer = layerStack.PopLayer();
		if (layer)
			layer->OnDetach();
		return layer;
	}

	Layer* Application::PopOverlay()
	{
		Layer* overlay = layerStack.PopOverlay();
		if (overlay)
			overlay->OnDetach();
		return overlay;
	}

	void Application::OnEvent(Event& event)
	{
		GBC_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);

		// All of Input's event methods return false, so the event is never handled.
		dispatcher.Dispatch(&Input::OnKeyPressEvent);
		dispatcher.Dispatch(&Input::OnKeyReleaseEvent);
		dispatcher.Dispatch(&Input::OnMouseButtonPressEvent);
		dispatcher.Dispatch(&Input::OnMouseButtonReleaseEvent);

		dispatcher.Dispatch(this, &Application::OnWindowFramebufferResizeEvent);
		dispatcher.Dispatch(this, &Application::OnWindowFocusEvent);
		dispatcher.Dispatch(this, &Application::OnWindowMinimizeEvent);

#if GBC_ENABLE_IMGUI
		if (event.IsInCategory(EventCategory_Keyboard | EventCategory_Mouse))
			imguiWrapper->OnEvent(event);
#endif

		for (auto it = layerStack.rbegin(); !event.handled && it != layerStack.rend(); ++it)
		{
			Layer* layer = *it;
			if (layer->IsEnabled())
				layer->OnEvent(event);
		}

		// Let the client handle window close events if they want to.
		if (!event.handled)
			dispatcher.Dispatch(this, &Application::OnWindowCloseEvent);
	}

	void Application::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		if (focusedNativeWindow == event.GetNativeWindow())
			focusedNativeWindow = nullptr;

		// TODO: shouldn't close when a window closes.
		// For example, multiwindow applications, one closes and it closes the entire application. Not supposed to happen.
		Close();
		event.handled = true;
	}

	void Application::OnWindowFramebufferResizeEvent(WindowFramebufferResizeEvent& event)
	{
		GBC_PROFILE_FUNCTION();

		windowZeroSize = event.GetWidth() <= 0 || event.GetHeight() <= 0;
		if (windowZeroSize)
		{
			rendering = false;
			// Don't dispatch the event to resize with zero size
			event.handled = true;
		}
		else
		{
			rendering = true;
			Renderer::SetViewport(0, 0, event.GetWidth(), event.GetHeight());
		}
	}

	void Application::OnWindowFocusEvent(WindowFocusEvent& event)
	{
		if (event.IsFocused())
			focusedNativeWindow = event.GetNativeWindow();
	}

	void Application::OnWindowMinimizeEvent(WindowMinimizeEvent& event)
	{
		rendering = !(windowZeroSize || event.IsMinimized());
	}
}
