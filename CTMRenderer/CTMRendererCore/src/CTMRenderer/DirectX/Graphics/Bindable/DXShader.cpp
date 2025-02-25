#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/Bindable/DXShader.hpp"

#include <d3d11.h>
#include <wrl/client.h>

namespace CTMRenderer::CTMDirectX::Graphics::Bindable
{
	DXShaderSource::DXShaderSource(
		ImplementedShaderType shaderType,
		Microsoft::WRL::ComPtr<ID3DBlob> pVSBytecode,
		Microsoft::WRL::ComPtr<ID3DBlob> pPSBytecode) noexcept
		: m_ShaderType(shaderType),
		  mP_VSBytecode(std::move(pVSBytecode)),
		  mP_PSBytecode(std::move(pPSBytecode))
	{
	}
}