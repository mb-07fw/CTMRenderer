#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMRenderer.hpp"

namespace CTMRenderer
{
	void CTMRenderer::Start() noexcept
	{
		m_Renderer->Start();
	}

	void CTMRenderer::Shutdown() noexcept
	{
		m_Renderer->Shutdown();
	}

	void CTMRenderer::JoinForShutdown() noexcept
	{
		m_Renderer->JoinForShutdown();
	}

	void CTMRenderer::ClearScreen() noexcept
	{
		RUNTIME_ASSERT(m_Renderer->IsRunning(), "Graphics specific operations such as clearing the screen can only be down when CTMRenderer is running.\n");

		m_Renderer->ClearScreen();
	}
}