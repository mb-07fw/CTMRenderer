#pragma once

namespace CTMRenderer::CTMDirectX
{
	struct DXRendererSettings
	{
		inline DXRendererSettings(unsigned int targetFPS)
			: TargetFPS(targetFPS) {}

		unsigned int TargetFPS;
	};
}