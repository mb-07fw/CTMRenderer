#include "Core/CorePCH.hpp"
#include "CTMRenderer/ICTMRenderer.hpp"

namespace CTMRenderer
{
	void ICTMRenderer::SubmitShape(const Shapes::CTMShape& shapeRef) noexcept
	{
		//DEBUG_PRINT("(ICTMRenderer:SubmitShape) Submitted shape : " << shapeRef.TypeToStr() << '\n');

		m_DrawQueue.QueueShape(shapeRef);
	}
}