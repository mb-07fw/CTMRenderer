workspace "CTMRenderer"
	architecture "x64"
	configurations { "Debug", "Release" }
	language "C++"
	cppdialect "C++20"

	local outputdir = ("%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")
	local target_dir = "bin/out/" .. outputdir ..  "/%{prj.name}"

	targetdir (target_dir)
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	defines {
		'OUTPUT_DIR="out/' .. outputdir .. '/%{prj.name}/"'
	}

	filter "configurations:Debug"
			defines { "DEBUG_MODE" }
			symbols "On"
	filter{} -- clear filters

	filter "configurations:Release"
		optimize "On"
	filter{} -- clear filters

	project "CTMRendererApp"
		locationdir = "CTMRenderer/CTMRendererApp/"

		location (locationdir)
		kind "ConsoleApp" -- Set the subsystem as a console application.

		files { locationdir .. "src/**.cpp", locationdir .. "include/**.hpp" }
		includedirs { locationdir .. "include/" }

		links { "CTMRendererCore.lib" }
		libdirs { "bin/out/" .. outputdir .. "/CTMRendererCore/" }

		dependson { "CTMRendererCore" }
		includedirs { "CTMRenderer/CTMRendererCore/include" }

	project "CTMRendererCore"
		locationdir = "CTMRenderer/CTMRendererCore/"
		shaderdir = locationdir .. "resources/shaders/"
		
		location (locationdir)
		kind "StaticLib" -- Compile as a static library.

		files { 
			locationdir .. "src/**.cpp", 
			locationdir .. "include/**.hpp",
			shaderdir .. "/**.hlsl"
		}

		filter { "files:**VS.hlsl" }
			shadertype "Vertex"
			shadermodel "4.0"
			entrypoint "main"
		filter{} -- clear filters.

		filter { "files:**PS.hlsl" }
			shadertype "Pixel"
			shadermodel "4.0"
			entrypoint "main"
		filter{} -- clear filters.

		includedirs { locationdir .. "include/" }

		-- These are currently included via #pragma comment's. in RendererCore.hpp.
		-- links { "d3d11.lib", "d3dcompiler.lib", "D2d1.lib", "dwrite.lib" }

		pchheader "Core/CorePCH.hpp" -- Define how the header is included.
		pchsource (locationdir .. "src/Core/CorePCH.cpp") -- Define the path of the pch source file.
