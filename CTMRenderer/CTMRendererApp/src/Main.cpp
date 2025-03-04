#include "CTMRenderer/CTMRenderer.hpp"

#include <chrono>
#include <iostream>

int main()
{
	using namespace CTMRenderer::Shapes;

	CTMRenderer::CTMRenderer renderer(CTMRenderer::CTMRendererType::CTM_DIRECTX, 60u);

	renderer.Start();

	renderer.ClearScreen();

	std::shared_ptr<CTMRect> pRect = renderer.MakeRect(50.0f, 50.0f, 60.0f, 60.0f, CTMColor(CTMColorType::RED));
	renderer.SubmitShape(*pRect);

	renderer.JoinForShutdown();

	std::cout << "Main end.\n";
}