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

	Shapes::IRectangle CTMRenderer::MakeRect(float left, float top, float right, float bottom, Shapes::Color color) const noexcept
	{
		return m_Renderer->MakeRect(left, top, right, bottom, color);
	}


	void CTMRenderer::SubmitShape(const Shapes::IShape& shape) noexcept
	{
		m_Renderer->SubmitShape(shape);
	}
}