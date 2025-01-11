#include "RendererCore.hpp"

int main()
{
	Renderer::Renderer renderer;

	renderer.Start();

	renderer.JoinForShutdown();
}