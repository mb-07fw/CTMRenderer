#pragma once

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/DirectX/Graphics/Geometry/DXAABB.hpp"
#include "CTMRenderer/DirectX/Graphics/DXColor.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXShader.hpp"

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
		IShape(ShapeType type, Bindable::ImplementedVSType respectiveVSType, Bindable::ImplementedPSType respectivePSType) noexcept;
		virtual ~IShape() = default;
	public:
		[[nodiscard]] ShapeType Type() const noexcept { return m_Type; }
		[[nodiscard]] Bindable::ImplementedVSType VSType() const noexcept { return m_VSType; }
		[[nodiscard]] Bindable::ImplementedPSType PSType() const noexcept { return m_PSType; }
	private:
		const ShapeType m_Type;
		const Bindable::ImplementedVSType m_VSType;
		const Bindable::ImplementedPSType m_PSType;
	};

	class DXRect : public IShape
	{
	public:
		DXRect(float left, float top, float right, float bottom, DXColor color) noexcept;
		~DXRect() = default;
	public:
		inline [[nodiscard]] DXAABB& AABB() noexcept { return m_AABB; }
		inline [[nodiscard]] DXColor Color() noexcept { return m_Color; }
	private:
		DXAABB m_AABB;
		DXColor m_Color;
	};
}