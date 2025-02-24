#pragma once

#include <vector>
#include <filesystem>

namespace CTMRenderer::CTMDirectX::Graphics
{
	class DXShaderLibrary
	{
	public:
		DXShaderLibrary() noexcept;
		~DXShaderLibrary() = default;
	public:
	private:
		void LoadShaders() noexcept;
	private:
		std::vector<std::filesystem::path> m_Shaders;
	};
}