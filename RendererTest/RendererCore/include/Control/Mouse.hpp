#pragma once

namespace CTMRenderer::Control
{
	// A data representation of a mouse.
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
		int m_PosX = 0, m_PosY = 0;
	};
}