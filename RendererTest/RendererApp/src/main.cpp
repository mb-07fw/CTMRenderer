#include "Core/RendererCore.hpp"
#include <iostream>

int main()
{
	Renderer::Renderer renderer(60u);

	renderer.Start();
	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}