#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/DXShaderLibrary.hpp"
#include "Core/CoreUtility.hpp"

#include <d3d11.h>

namespace CTMRenderer::CTMDirectX::Graphics
{
	DXShaderLibrary::DXShaderLibrary() noexcept
        : m_ShaderPath(std::move(Utility::GetOutDirectory()))
	{
        RUNTIME_ASSERT(std::filesystem::exists(m_ShaderPath), "Shader path `" << m_ShaderPath << "` not found; program cannot continue.\n");

        DEBUG_PRINT("Shader path : " << m_ShaderPath << '\n');

		LoadShaders();
	}

	void DXShaderLibrary::LoadShaders() noexcept
	{
        try
        {
            Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
            std::vector<Bindable::DXShaderData> shaders; // This only stores one of both vertex and pixel shader.

            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(m_ShaderPath))
            {
                if (!std::filesystem::is_regular_file(entry)) // Ensure it's a file, not a directory
                    continue;

                std::filesystem::path extension = std::move(entry.path().extension());

                if (extension != ".cso")
                    continue;

                std::filesystem::path fileName = std::move(entry.path().filename());
                std::wstring fileNameWStr = fileName.wstring();

                std::wstring shaderFlag = fileNameWStr.substr(fileNameWStr.size() - 6, 2);
                RUNTIME_ASSERT(shaderFlag == L"VS" || shaderFlag == L"PS", "Invalid file `" << fileName << "` in shader directory : " << m_ShaderPath << '\n');

                std::wstring shaderName = fileNameWStr.substr(0, fileNameWStr.size() - 6);

                Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
                std::wstring shaderPath = entry.path().wstring();

                HRESULT hResult = D3DReadFileToBlob(shaderPath.c_str(), pBlob.GetAddressOf());
                RUNTIME_ASSERT(hResult == S_OK, "Failed to read shader path : " << entry.path() << '\n');

                DEBUG_PRINTW("Successfully read in shader : " << shaderName << '\n');

                //shaders.emplace_back(std::move(shaderName));
            }

            DEBUG_PRINT("Shaders collected : " << m_VertexShaders.size() + m_PixelShaders.size() << '\n');
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            DEBUG_PRINT_ERROR("Filesystem error: " << e.what() << '\n');
        }
	}

    [[nodiscard]] Bindable::ImplementedShaderType DXShaderLibrary::GetImplementedShaderType(const std::wstring& shaderName) noexcept
    {
        if (shaderName == L"Pos2_Def")
            return Bindable::ImplementedShaderType::POS2_DEF;
        else
            RUNTIME_ASSERTW(false, "No matching vertex shader type found for shader named : " << shaderName << '\n');
    }
}