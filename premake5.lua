workspace "RendererTest"
	architecture "x64"
	configurations { "Debug", "Release" }
	language "C++"
	cppdialect "C++20"

	local outputdir = ("%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}")

	targetdir ("bin/out/" .. outputdir ..  "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	filter "configurations:Debug"
			defines { "DEBUG_MODE" }
			symbols "On"
		filter{} -- clear filters

	filter "configurations:Release"
		optimize "On"
	filter{} -- clear filters

	project "RendererApp"
		location "RendererTest/RendererApp"
		kind "ConsoleApp" -- Set the subsystem as a console application.

		locationdir = "RendererTest/RendererApp/"

		files { locationdir .. "src/**.cpp", locationdir .. "include/**.hpp" }
		includedirs { locationdir .. "include/" }
		links { "RendererCore" }
		libdirs { "bin/out/" .. outputdir .. "/RendererCore/" }
		dependson { "RendererCore" }
		includedirs { "RendererTest/RendererCore/include" }

	project "RendererCore"
		locationdir = "RendererTest/RendererCore/"
		
		location (locationdir)
		kind "StaticLib"

		files { locationdir .. "src/**.cpp", locationdir .. "include/**.hpp" }
		includedirs { locationdir .. "include/" }

		pchheader "Core/CorePCH.hpp"	-- Define how the header is included.
		pchsource (locationdir .. "src/Core/CorePCH.cpp") -- Define the path of the pch source file.