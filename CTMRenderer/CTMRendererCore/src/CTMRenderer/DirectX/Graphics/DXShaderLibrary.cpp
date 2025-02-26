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
            std::vector<Bindable::DXShaderData> shaderData;
            shaderData.reserve(5);

            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(m_ShaderPath))
            {
                if (!std::filesystem::is_regular_file(entry)) // Ensure it's a file, not a directory
                    continue;

                std::filesystem::path extension = std::move(entry.path().extension());
                std::wstring pathWStr = entry.path().wstring();

                if (extension != ".cso")
                    continue;

                std::filesystem::path fileName = std::move(entry.path().filename());
                std::wstring fileNameWStr = fileName.wstring();

                std::wstring shaderFlag = fileNameWStr.substr(fileNameWStr.size() - 6, 2);
                RUNTIME_ASSERT(shaderFlag == S_VERTEX_FLAG || shaderFlag == S_PIXEL_FLAG, "Invalid shader flag for file : " << fileName << '\n');

                std::wstring shaderName = fileNameWStr.substr(0, fileNameWStr.size() - 6);
                DEBUG_PRINTW("Shader name : " << shaderName << '\n');

                if (shaderName != L"Pos2_Def")
                    continue;

                Bindable::ImplementedShaderType shaderType = GetShaderType(shaderName);

                switch (shaderType)
                {
                case Bindable::ImplementedShaderType::POS2_DEF:
                    DEBUG_PRINT("Found Pos2_Def.\n");
                    break;
                }

                bool foundDuplicate = false;
                for (const Bindable::DXShaderData& data : shaderData)
                {
                    if (data.ShaderType == shaderType)
                    {
                        foundDuplicate = true;
                        DEBUG_PRINTW("Skipping already collected shader : " << shaderName << '\n');
                        break;
                    }
                }

                if (foundDuplicate)
                    continue;

                Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
                Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;

                Microsoft::WRL::ComPtr<ID3DBlob>& currentBlob = pVSBlob;

                HRESULT hResult = S_OK;
                if (shaderFlag == S_VERTEX_FLAG)
                {
                    hResult = D3DReadFileToBlob(pathWStr.c_str(), pVSBlob.GetAddressOf());
                    RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                    pathWStr[pathWStr.size() - 6] = 'P';

                    hResult = D3DReadFileToBlob(pathWStr.c_str(), pPSBlob.GetAddressOf());
                    RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                    shaderData.emplace_back(shaderType, pVSBlob, pPSBlob);

                    DEBUG_PRINTW("Emplaced shader : " << shaderName << '\n');
                }
                else if (shaderFlag == S_PIXEL_FLAG)
                {
                    hResult = D3DReadFileToBlob(pathWStr.c_str(), pPSBlob.GetAddressOf());
                    RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                    pathWStr[pathWStr.size() - 6] = 'V';

                    hResult = D3DReadFileToBlob(pathWStr.c_str(), pVSBlob.GetAddressOf());
                    RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                    shaderData.emplace_back(shaderType, pVSBlob, pPSBlob);

                    DEBUG_PRINTW("Emplaced shader : " << shaderName << '\n');
                }
                else
                    RUNTIME_ASSERTW(false, "Unhandled shader : " << shaderName << '\n');
            }

            DEBUG_PRINT("Shader data : " << shaderData.size() << '\n');
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            DEBUG_PRINT_ERROR("Filesystem error: " << e.what() << '\n');
        }
    }

    [[nodiscard]] Bindable::ImplementedShaderType DXShaderLibrary::GetShaderType(const std::wstring& shaderName) noexcept
    {
        if (shaderName == L"Pos2_Def")
            return Bindable::ImplementedShaderType::POS2_DEF;

        RUNTIME_ASSERTW(false, "Couldn't find match enum type for shader : " << shaderName << '\n');
    }
}