#include "CTMRenderer/CTMRenderer.hpp"

#include <chrono>

#include <iostream>

int main()
{
	using namespace CTMRenderer::Shapes;

	CTMRenderer::CTMRenderer renderer(CTMRenderer::CTMRendererType::CTM_DIRECTX, 60u);

	renderer.Init();

	renderer.ClearScreen();

	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}