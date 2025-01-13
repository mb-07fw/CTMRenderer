#include "CorePCH.hpp"
#include "Renderer.hpp"
#include "RendererMacros.hpp"

namespace Renderer
{
	Renderer::Renderer()
		: m_Window(), m_EventManager()
	{
		DEBUG_PRINT("[Renderer] Initialized renderer.\n");
	}

	void Renderer::Start()
	{
		const Event::EventListener* listener = m_EventManager.GetActiveListener(Event::ListenType::LISTEN_ALL);
		
		if (listener->IsActive())
			DEBUG_PRINT("[Renderer PUB.Start] Initialized listener : " + std::to_string(listener->ID()) + '\n');

		DEBUG_PRINT("[Renderer PUB.Start] Is notified : " + Utility::BoolToString(listener->IsNotified()) + '\n');

		const Event::Event* event = listener->CurrentEvent();
		if (event != nullptr)
			DEBUG_PRINT("Renderer PUB.Start] Event : " + Event::Event::TypeToString(event->type) + '\n');

		m_EventManager.PushEvent(Event::EventType::RENDERER_START);

		DEBUG_PRINT("[Renderer PUB.Start] Is notified : " + Utility::BoolToString(listener->IsNotified()) + '\n');

		event = listener->CurrentEvent();
		if (event != nullptr)
			DEBUG_PRINT("[Renderer PUB.Start] Event : " + Event::Event::TypeToString(event->type) + '\n');

		// TODO : Create the window the on the same thread that processes its messages.
		//m_Window.StartMessageLoop();
	}

	void Renderer::JoinForShutdown()
	{
		m_Window.JoinMessageLoop();
	}
}