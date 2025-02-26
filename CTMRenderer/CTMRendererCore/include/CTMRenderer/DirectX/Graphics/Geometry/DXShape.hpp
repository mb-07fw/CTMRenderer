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
		DXRect(float left, float top, float right, float bottom, DXColor color) noexcept;
		~DXRect() = default;
	public:
		virtual inline [[nodiscard]] ShapeType Type() const noexcept override { return ShapeType::RECT; }
		inline [[nodiscard]] DXAABB& AABB() noexcept { return m_AABB; }
		inline [[nodiscard]] DXColor Color() noexcept { return m_Color; }
	private:
		DXAABB m_AABB;
		DXColor m_Color;
	};
}