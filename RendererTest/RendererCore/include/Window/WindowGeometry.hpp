#pragma once

#include "Core/CoreMacros.hpp"

namespace CTMRenderer::Window::Geometry
{
	struct WindowArea
	{
		inline WindowArea(unsigned int width, unsigned int height)
			: width(width), height(height)
		{
			RUNTIME_ASSERT(width != 0, "Width cannot be zero.\n");
			RUNTIME_ASSERT(height != 0, "Height cannot be zero.\n");

			aspectRatio = (float)width / height;
			aspectRatioReciprocal = (float)height / width;
		}

		unsigned int width, height;
		float aspectRatio, aspectRatioReciprocal;
	};
}