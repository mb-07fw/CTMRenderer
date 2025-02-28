#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMRenderer.hpp"

namespace CTMRenderer
{
	void CTMRenderer::Init() noexcept
	{
		m_Renderer->Init();
	}

	void CTMRenderer::JoinForShutdown() noexcept
	{
		m_Renderer->JoinForShutdown();
	}

	void CTMRenderer::ClearScreen() noexcept
	{
		m_Renderer->ClearScreen();
	}

	void CTMRenderer::SubmitShape(const Shapes::CTMShape& shape) noexcept
	{
		m_Renderer->SubmitShape(shape);
	}
}