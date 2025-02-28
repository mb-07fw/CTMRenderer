#include "Core/CorePCH.hpp"
#include "CTMRenderer/CTMRenderer.hpp"

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

	void CTMRenderer::SubmitShape(const Shapes::CTMShape& shape) noexcept
	{
		m_Renderer->SubmitShape(shape);
	}
}