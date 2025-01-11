#pragma once

#include "Window.hpp"

namespace Renderer
{
	class Renderer
	{
	public:
		Renderer();
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;
	public:
		void Start();
		void JoinForShutdown();
	private:
		Window::Window m_Window;
	};
}