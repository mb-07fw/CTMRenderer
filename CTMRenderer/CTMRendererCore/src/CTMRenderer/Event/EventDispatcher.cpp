#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/EventDispatcher.hpp"

namespace CTMRenderer::Event
{
	EventDispatcher::EventDispatcher() noexcept
	{
	}

	void EventDispatcher::Subscribe(IGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved generic listener is nullptr.");

		m_GenericListeners[pGenericListener->ListenType()].emplace_back(pGenericListener);
	}
	
	void EventDispatcher::Subscribe(IConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		m_ConcreteListeners[pConcreteListener->ListenType()].emplace_back(pConcreteListener);
	}

	void EventDispatcher::Unsubscribe(IGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<IGenericListener*>& genericListeners = m_GenericListeners[pGenericListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < genericListeners.size(); ++i)
			if (pGenericListener == genericListeners[i])
				genericListeners.erase(genericListeners.begin() + i);
	}

	void EventDispatcher::Unsubscribe(IConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<IConcreteListener*>& concreteListeners = m_ConcreteListeners[pConcreteListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < concreteListeners.size(); ++i)
			if (pConcreteListener == concreteListeners[i])
				concreteListeners.erase(concreteListeners.begin() + i);
	}

	void EventDispatcher::DispatchQueued() noexcept
	{
		// Iterate through each entry in the map.
		for (auto& [key, value] : m_EventQueues)
		{
			// Iterate through each event in the ConcreteEventType's mapped event queue.
			for (IEvent* pEvent : value)
				switch (pEvent->ConcreteType())
				{
				case ConcreteEventType::CTM_STATE_START_EVENT:
					DispatchEvent(StartEvent::Cast(pEvent));
					break;
				case ConcreteEventType::CTM_STATE_END_EVENT:
					DispatchEvent(EndEvent::Cast(pEvent));
					break;
				case ConcreteEventType::CTM_MOUSE_MOVE_EVENT:
					DispatchEvent(MouseMoveEvent::Cast(pEvent));
					break;
				case ConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT:
					DispatchEvent(ClearFrameEvent::Cast(pEvent));
					break;
				case ConcreteEventType::CTM_FRAME_START_FRAME_EVENT:
					DispatchEvent(StartFrameEvent::Cast(pEvent));
					break;
				case ConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT:
					DispatchEvent(DrawFrameEvent::Cast(pEvent));
					break;
				}

			// Flush event queue.
			value.clear();
		}

		m_IsEventQueued.store(false, std::memory_order_release);
	}
}