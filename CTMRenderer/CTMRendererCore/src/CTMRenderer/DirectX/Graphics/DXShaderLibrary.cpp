#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/DXShaderLibrary.hpp"
#include "Core/CoreUtility.hpp"

#include <chrono>

namespace CTMRenderer::CTMDirectX::Graphics
{
    DXShaderLibrary::DXShaderLibrary() noexcept
        : m_ShaderPath(std::move(Utility::GetOutDirectory()))
    {
        RUNTIME_ASSERT(std::filesystem::exists(m_ShaderPath), "Shader path `" << m_ShaderPath << "` not found; program cannot continue.\n");

        DEBUG_PRINT("Shader path : " << m_ShaderPath << '\n');

        LoadShaders();
    }

    [[nodiscard]] const Bindable::DXShaderData& DXShaderLibrary::GetData(Bindable::ImplementedShaderType type) const noexcept
    {
        RUNTIME_ASSERT(type != Bindable::ImplementedShaderType::INVALID, "Cannot retrieve an invalid shader type.\n");
        RUNTIME_ASSERT(m_ShaderData.size() != 0, "No shaders to retrieve.\n");

        for (const Bindable::DXShaderData& data : m_ShaderData)
            if (data.ShaderType == type)
                return data;

        RUNTIME_ASSERT(false, "");
    }

    [[nodiscard]] const wchar_t* DXShaderLibrary::ShaderTypeToWStr(Bindable::ImplementedShaderType type) noexcept
    {
        switch (type)
        {
        case Bindable::ImplementedShaderType::POS2_DEF: return L"Pos2_Def";
        case Bindable::ImplementedShaderType::INVALID:
        default: 
            RUNTIME_ASSERT(false, "Cannot convert an invalid shader type to a wchar_t const*.\n");
            return nullptr;
        }
    }

    [[nodiscard]] const char* DXShaderLibrary::ShaderTypeToStr(Bindable::ImplementedShaderType type) noexcept
    {
        switch (type)
        {
        case Bindable::ImplementedShaderType::POS2_DEF: return "Pos2_Def";
        case Bindable::ImplementedShaderType::INVALID:
        default: 
            RUNTIME_ASSERT(false, "Cannot convert an invalid shader type to a char const*.\n");
            return nullptr;
        }
    }

    void DXShaderLibrary::LoadShaders() noexcept
    {
        try
        {
            size_t fileCount = 0;

            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(m_ShaderPath))
            {
                if (!std::filesystem::is_regular_file(entry)) // Ensure it's a file, not a directory
                    continue;

                ++fileCount;

                std::filesystem::path extension = std::move(entry.path().extension());
                std::wstring pathWStr = entry.path().wstring();

                if (extension != ".cso")
                    continue;

                std::filesystem::path fileName = std::move(entry.path().filename());
                std::wstring fileNameWStr = fileName.wstring();

                std::wstring shaderFlag = fileNameWStr.substr(fileNameWStr.size() - 6, 2);
                RUNTIME_ASSERT(shaderFlag == S_VERTEX_FLAG || shaderFlag == S_PIXEL_FLAG, "Invalid shader flag for file : " << fileName << '\n');

                std::wstring shaderName = fileNameWStr.substr(0, fileNameWStr.size() - 6);
                //DEBUG_PRINTW("Shader name : " << shaderName << '\n');

                if (shaderName != L"Pos2_Def")
                    continue;

                Bindable::ImplementedShaderType shaderType = GetShaderType(shaderName);

                switch (shaderType)
                {
                case Bindable::ImplementedShaderType::POS2_DEF:
                    //DEBUG_PRINT("Found Pos2_Def.\n");
                    break;
                }

                // If m_ShaderData already contains the type, (e.g., already read in the shader's counterpart)
                bool foundDuplicate = false;
                for (const Bindable::DXShaderData& data : m_ShaderData)
                    if (data.ShaderType == shaderType)
                    {
                        foundDuplicate = true;
                        //DEBUG_PRINTW("Skipping already collected shader : " << shaderName << '\n');
                        break;
                    }

                // Skip the shader if it's name has already been read.
                if (foundDuplicate)
                    continue;

                // Change the flag from PS to VS to read in the vertex shader first.
                if (shaderFlag == S_PIXEL_FLAG)
                    pathWStr[pathWStr.size() - 6] = 'V';

                Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
                Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;

                HRESULT hResult = S_OK;
                hResult = D3DReadFileToBlob(pathWStr.c_str(), pVSBlob.GetAddressOf());
                RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                pathWStr[pathWStr.size() - 6] = 'P';

                hResult = D3DReadFileToBlob(pathWStr.c_str(), pPSBlob.GetAddressOf());
                RUNTIME_ASSERTW(hResult == S_OK, "Failed to read in file : " << pathWStr << '\n');

                m_ShaderData.emplace_back(shaderType, pVSBlob, pPSBlob);

                //DEBUG_PRINTW("Emplaced shader : " << shaderName << '\n');
            }

            DEBUG_PRINT("Total files : " << fileCount << '\n');
            DEBUG_PRINT("Total data : " << m_ShaderData.size() << '\n');
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
        return Bindable::ImplementedShaderType::INVALID;
    }
}