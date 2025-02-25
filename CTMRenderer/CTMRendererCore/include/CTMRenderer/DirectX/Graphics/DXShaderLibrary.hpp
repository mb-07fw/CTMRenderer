#pragma once

#include <vector>
#include <filesystem>

#include "CTMRenderer/DirectX/Graphics/Bindable/DXShader.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	class DXShaderLibrary
	{
	public:
		static constexpr wchar_t const* S_VERTEX_FLAG = L"VS";
		static constexpr wchar_t const* S_PIXEL_FLAG = L"PS";
	public:
		DXShaderLibrary() noexcept;
		~DXShaderLibrary() = default;
	public:
	private:
		void LoadShaders() noexcept;
		static [[nodiscard]] Bindable::ImplementedShaderType GetImplementedShaderType(const std::wstring& shaderName) noexcept;
	private:
		const std::filesystem::path m_ShaderPath;
		std::vector<std::wstring> m_VertexShaders;
		std::vector<std::wstring> m_PixelShaders;
	};
}