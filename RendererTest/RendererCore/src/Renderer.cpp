#include "CorePCH.hpp"
#include "Renderer.hpp"
#include "RendererMacros.hpp"

namespace Renderer
{
	Renderer::Renderer()
		: m_Window()
	{
		DEBUG_PRINT("Initialized renderer.\n");
	}

	void Renderer::Start()
	{
		m_Window.StartMessageLoop();
	}

	void Renderer::JoinForShutdown()
	{
		// TODO : Call a funcion to join Window's message loop thread.
		//   Pseudo ---> m_Window.JoinMessageLoop();
	}
}