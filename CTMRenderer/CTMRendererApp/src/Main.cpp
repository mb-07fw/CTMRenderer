#include "CTMRenderer/CTMRenderer.hpp"

#include <iostream>

int main()
{
	CTMRenderer::CTMRenderer renderer(CTMRenderer::RendererType::CTM_DIRECTX, 60u);

	renderer.Start();

	//renderer.SubmitShape()

	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}