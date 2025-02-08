#include "Core/RendererCore.hpp"
#include <iostream>

int main()
{
	CTMRenderer::CTMRenderer renderer(60u);

	renderer.Start();
	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}