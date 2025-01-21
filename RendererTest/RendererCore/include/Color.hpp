#pragma once

namespace Renderer::Window::Graphics
{
	struct Color
	{
	public:
		inline Color(float r, float g, float b, float a = 1.0f)
		{
			rgba[0] = r;
			rgba[1] = g;
			rgba[2] = b;
			rgba[3] = a;
		}
	public:
		inline constexpr float R() const noexcept { return rgba[0]; }
		inline constexpr float G() const noexcept { return rgba[1]; }
		inline constexpr float B() const noexcept { return rgba[2]; }
		inline constexpr float A() const noexcept { return rgba[3]; }
	public:
		float rgba[4];
	};
}