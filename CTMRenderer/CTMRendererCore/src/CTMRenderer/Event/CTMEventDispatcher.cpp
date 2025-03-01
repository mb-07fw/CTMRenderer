#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/CTMEventDispatcher.hpp"

namespace CTMRenderer::Event
{
	CTMEventDispatcher::CTMEventDispatcher() noexcept
	{
	}

	void CTMEventDispatcher::Subscribe(ICTMGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved generic listener is nullptr.");

		m_GenericListeners[pGenericListener->ListenType()].emplace_back(pGenericListener);
	}
	
	void CTMEventDispatcher::Subscribe(ICTMConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		m_ConcreteListeners[pConcreteListener->ListenType()].emplace_back(pConcreteListener);
	}

	void CTMEventDispatcher::Unsubscribe(ICTMGenericListener* pGenericListener) noexcept
	{
		RUNTIME_ASSERT(pGenericListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<ICTMGenericListener*>& genericListeners = m_GenericListeners[pGenericListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < genericListeners.size(); ++i)
			if (pGenericListener == genericListeners[i])
				genericListeners.erase(genericListeners.begin() + i);
	}

	void CTMEventDispatcher::Unsubscribe(ICTMConcreteListener* pConcreteListener) noexcept
	{
		RUNTIME_ASSERT(pConcreteListener != nullptr, "Recieved concrete listener is nullptr.");

		std::vector<ICTMConcreteListener*>& concreteListeners = m_ConcreteListeners[pConcreteListener->ListenType()];

		// If the pointer of the provided listener matches the stored listener, remove it from storage.
		for (size_t i = 0; i < concreteListeners.size(); ++i)
			if (pConcreteListener == concreteListeners[i])
				concreteListeners.erase(concreteListeners.begin() + i);
	}

	void CTMEventDispatcher::DispatchQueued() noexcept
	{
		// Iterate through each entry in the map.
		for (auto& [key, value] : m_EventQueues)
		{
			if (value.empty())
				continue;

			// Iterate through each event in the ConcreteEventType's mapped event queue.
			for (ICTMEvent* pEvent : value)
				switch (pEvent->ConcreteType())
				{
				case CTMConcreteEventType::CTM_STATE_START_EVENT:
					DispatchEvent(CTMStartEvent::Cast(pEvent));
					break;
				case CTMConcreteEventType::CTM_STATE_END_EVENT:
					DispatchEvent(CTMEndEvent::Cast(pEvent));
					return; // Skip processing other events after the CTMEndEvent was dispatched.
				case CTMConcreteEventType::CTM_MOUSE_MOVE_EVENT:
					DispatchEvent(CTMMouseMoveEvent::Cast(pEvent));
					break;
				case CTMConcreteEventType::CTM_FRAME_CLEAR_FRAME_EVENT:
					DispatchEvent(CTMClearFrameEvent::Cast(pEvent));
					break;
				case CTMConcreteEventType::CTM_FRAME_START_FRAME_EVENT:
					DispatchEvent(CTMStartFrameEvent::Cast(pEvent));
					break;
				case CTMConcreteEventType::CTM_FRAME_DRAW_FRAME_EVENT:
					DispatchEvent(CTMDrawFrameEvent::Cast(pEvent));
					break;
				}

			// Flush event queue.
			value.clear();
		}

		m_IsEventQueued.store(false, std::memory_order_release);
	}
}