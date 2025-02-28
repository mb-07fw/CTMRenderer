#include "CTMRenderer/CTMRenderer.hpp"

#include <iostream>

int main()
{
	using namespace CTMRenderer::Shapes;

	CTMRenderer::CTMRenderer renderer(CTMRenderer::CTMRendererType::CTM_DIRECTX, 60u);

	//CTMRect rect(0, 0, 1, 1, CTMColor(CTMColorType::BLUE));

	renderer.Init();

	renderer.ClearScreen();

	//renderer.SubmitShape(rect);

	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}