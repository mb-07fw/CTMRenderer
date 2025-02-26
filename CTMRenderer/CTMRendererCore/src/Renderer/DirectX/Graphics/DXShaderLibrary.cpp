#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/DXShaderLibrary.hpp"
#include "Core/CoreUtility.hpp"

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

                if (shaderFlag == L"VS")
                    m_VertexShaders.emplace_back(std::move(fileNameWStr));
                else if (shaderFlag == L"PS")
                    m_PixelShaders.emplace_back(std::move(fileNameWStr));
                else
                    DEBUG_PRINT("Invalid shader : " << fileName << '\n');

            }

            DEBUG_PRINT("Shaders collected : " << m_VertexShaders.size() + m_PixelShaders.size() << '\n');
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            DEBUG_PRINT_ERROR("Filesystem error: " << e.what() << '\n');
        }
	}
}