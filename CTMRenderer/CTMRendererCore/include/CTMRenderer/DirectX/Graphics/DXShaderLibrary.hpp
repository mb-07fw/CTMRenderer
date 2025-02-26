#pragma once

#include <vector>
#include <filesystem>

#include "CTMRenderer/DirectX/Graphics/Bindable/DXShader.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	class DXShaderLibrary
	{
	public:
		DXShaderLibrary() noexcept;
		~DXShaderLibrary() = default;
	public:
		static [[nodiscard]] const wchar_t* ShaderTypeToWStr(Bindable::ImplementedShaderType type) noexcept;
		static [[nodiscard]] const char* ShaderTypeToStr(Bindable::ImplementedShaderType type) noexcept;
		[[nodiscard]] const Bindable::DXShaderData& GetData(Bindable::ImplementedShaderType type) const noexcept;
	private:
		void LoadShaders() noexcept;
		static [[nodiscard]] Bindable::ImplementedShaderType GetShaderType(const std::wstring& shaderName) noexcept;
	private:
		static constexpr wchar_t const* S_VERTEX_FLAG = L"VS";
		static constexpr wchar_t const* S_PIXEL_FLAG = L"PS";
		const std::filesystem::path m_ShaderPath;
		std::vector<Bindable::DXShaderData> m_ShaderData;
	};
}