#pragma once

#include <vector>
#include <filesystem>

namespace CTMRenderer::CTMDirectX::Graphics
{
	enum class ImplementedShaderType
	{

	};

	class DXShaderLibrary
	{
	public:
		DXShaderLibrary() noexcept;
		~DXShaderLibrary() = default;
	public:
	private:
		void LoadShaders() noexcept;
	private:
		const std::filesystem::path m_ShaderPath;
		std::vector<std::wstring> m_VertexShaders;
		std::vector<std::wstring> m_PixelShaders;
	};
}