#pragma once

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXAABB.hpp"
#include "CTMRenderer/DirectX/Graphics/DXColor.hpp"

namespace CTMRenderer::CTMDirectX::Graphics::Geometry
{
	enum class ShapeType
	{
		RECT,
		CIRClE
	};

	class IShape
	{
	public:
		IShape(ShapeType type) noexcept;
		virtual ~IShape() = default;
	public:
		virtual [[nodiscard]] ShapeType Type() const noexcept = 0;
	private:
		const ShapeType m_Type;
	};

	class DXRect : public IShape
	{
	public:
		DXRect(float left, float top, float right, float bottom, const DXColor& color) noexcept;
		~DXRect() = default;
	public:
		[[nodiscard]] DXAABB& Aabb() noexcept;
	private:
		DXAABB AABB;
		DXColor color;
	};
}