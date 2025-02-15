#include "Renderer/CTMRenderer.hpp"
#include <iostream>

int main()
{
	CTMRenderer::CTMRenderer renderer(CTMRenderer::CTMRendererType::CTM_DIRECTX, 60u);

	renderer.Start();
	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}