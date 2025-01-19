#include "Core/RendererCore.hpp"
#include <iostream>

int main()
{
	Renderer::Renderer renderer;

	renderer.Start();

	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}