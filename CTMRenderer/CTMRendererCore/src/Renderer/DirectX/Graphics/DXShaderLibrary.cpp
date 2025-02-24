#include "Core/CorePCH.hpp"
#include "CTMRenderer/DirectX/Graphics/DXShaderLibrary.hpp"
#include "Core/CoreUtility.hpp"

namespace CTMRenderer::CTMDirectX::Graphics
{
	DXShaderLibrary::DXShaderLibrary() noexcept
	{
		LoadShaders();
	}

	void DXShaderLibrary::LoadShaders() noexcept
	{
        const std::filesystem::path shaderPath = Utility::GetOutDirectory();
        RUNTIME_ASSERT(std::filesystem::exists(shaderPath), "Shader path `" << shaderPath << "` not found; program cannot continue.\n")

        DEBUG_PRINT("Shader path : " << shaderPath << '\n');

        try
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(Utility::GetOutDirectory()))
                if (std::filesystem::is_regular_file(entry)) // Ensure it's a file, not a directory
                {
                    std::filesystem::path extension = entry.path().extension();

                    if (extension != ".cso")
                        continue;

                    DEBUG_PRINT("Entry is a shader.\n");
                    m_Shaders.emplace_back(std::move(entry.path().filename()));
                }

            DEBUG_PRINT("Shaders collected : " << m_Shaders.size() << '\n');
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            DEBUG_PRINT_ERROR("Filesystem error: " << e.what() << '\n');
        }
	}
}