#pragma once

namespace CTMRenderer::Window::Geometry
{
	struct WindowArea
	{
		inline WindowArea(unsigned int width, unsigned int height)
			: width(width), height(height) {}

		unsigned int width, height;
	};
}