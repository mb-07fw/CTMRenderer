#include "Core/CorePCH.hpp"
#include "CTMRenderer/Event/CTMEventListener.hpp"

namespace CTMRenderer::Event
{
	ICTMListener::ICTMListener(bool listensAbstract) noexcept
	{
		RUNTIME_ASSERT(listensAbstract || !listensAbstract, "How TF did you end up NOT passing a true or false bool?\n");

		m_ListensAbstract = listensAbstract;
		m_ListensConcrete = !m_ListensAbstract;
	}

	ICTMGenericListener::ICTMGenericListener(CTMGenericEventType listenType) noexcept
		: ICTMListener(true), m_ListenType(listenType)
	{
	}

	ICTMConcreteListener::ICTMConcreteListener(CTMConcreteEventType listenType) noexcept
		: ICTMListener(false), m_ListenType(listenType)
	{

	}
}