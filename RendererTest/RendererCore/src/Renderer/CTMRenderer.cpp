#include "Core/CorePCH.hpp"
#include "Renderer/CTMRenderer.hpp"

namespace CTMRenderer
{
	void CTMRenderer::Start() noexcept
	{
		m_Renderer->Start();
	}

	void CTMRenderer::JoinForShutdown() noexcept
	{
		m_Renderer->JoinForShutdown();
	}
}