#pragma once

#include "Color.hpp"

namespace Renderer::Window::Geometry
{
	struct WindowArea
	{
		WindowArea(unsigned int width, unsigned int height)
			: width(width), height(height) {
		}

		unsigned int width, height;
	};

	struct Pos2D
	{
		float x, y;

		inline static constexpr unsigned int Stride() noexcept
		{
			return sizeof(Pos2D);
		}
	};

	struct BareVertex2D
	{
		Pos2D pos;

		inline static constexpr unsigned int Stride() noexcept
		{
			return sizeof(BareVertex2D);
		}
	};

	struct alignas(16) BasicVertex2D
	{
		Pos2D pos;
		Graphics::Color color;

		inline static constexpr unsigned int Stride() noexcept
		{
			return sizeof(BasicVertex2D);
		}
	};
}