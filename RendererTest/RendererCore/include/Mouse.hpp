#pragma once

namespace CTMRenderer::Window::Control
{
	class Mouse
	{
	public:
		Mouse() = default;
		~Mouse() = default;
	public:
		inline void SetPos(int newPosX, int newPosY) noexcept
		{
			m_PosX = newPosX;
			m_PosY = newPosY;
		}

		inline [[nodiscard]] int PosX() const noexcept { return m_PosX; }
		inline [[nodiscard]] int PosY() const noexcept { return m_PosY; }
	private:
		int m_PosX, m_PosY;
	};
}