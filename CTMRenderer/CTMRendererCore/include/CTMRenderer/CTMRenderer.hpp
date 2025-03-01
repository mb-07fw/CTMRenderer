#pragma once

#include <memory>

#include "Core/CoreMacros.hpp"
#include "CTMRenderer/ICTMRenderer.hpp"
#include "CTMRenderer/CTMShape.hpp"

#ifdef CTM_NO_DX
#else

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include "DirectX/DXRenderer.hpp"

#endif

namespace CTMRenderer
{
	class CTMRenderer
	{
	public:
		CTMRenderer(CTMRendererType rendererType, unsigned int targetFPS = 60) noexcept
		{
			if (rendererType == CTMRendererType::CTM_DIRECTX)
			{
				#ifdef CTM_NO_DX
					RUNTIME_ASSERT(false, "CTMRenderer cannot be created with the DirectX API, as it was explicitly defined to not include it via the macro CTM_NO_DX.\n");
				#else
					DEBUG_PRINT("Creating Renderer with DirectX.\n");

					m_Renderer = std::make_unique<CTMDirectX::DXRenderer>(targetFPS);
				#endif
			}
			else
				RUNTIME_ASSERT(false, "Failed to initialize CTMRenderer due to the provided RendererType being unknown.\n");
		}

		void Init() noexcept;
		void JoinForShutdown() noexcept;
		void ClearScreen() noexcept;
		void Shutdown() noexcept;
		void SubmitShape(const Shapes::CTMShape& shape) noexcept;
	private:
		std::unique_ptr<ICTMRenderer> m_Renderer;
	};
}