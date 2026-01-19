workspace "Mineprint"
	configurations { "Debug", "Release", "Dist" }
	platforms { "Win64", "Linux" }
	startproject "Mineprint"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "Mineprint/Vendor/spdlog/include"
IncludeDir["SDL"] = "Mineprint/Vendor/SDL/Include"
IncludeDir["imgui"] = "Mineprint/Vendor/imgui/"
IncludeDir["glm"] = "Mineprint/Vendor/glm/Include"
IncludeDir["glad"] = "Mineprint/Vendor/glad/include"
IncludeDir["stb"] = "Mineprint/Vendor/stb"

group "Vendor"
    include "Mineprint/Vendor/imgui.lua"
group ""

project "Mineprint"
	cppdialect "C++20"
	kind "ConsoleApp"
	staticruntime "On"
	language "C++"
	characterset "Unicode"
	location "Mineprint"
	targetdir ("Build/%{prj.name}/" .. outputdir)
	objdir ("Build/%{prj.name}/Intermediates/" .. outputdir)
	debugdir ("Build/%{prj.name}/" .. outputdir)

	usestandardpreprocessor 'On'
	pchheader("mppch.h")
	pchsource "Mineprint/Source/mppch.cpp"

	vpaths {
		["Include"] = {"Mineprint/Include/**.h", "Mineprint/Include/**.hpp"},
		["Source"] = {"Mineprint/Source/**.cpp", "Mineprint/Source/**.c"},
	}

	files { 
		"Mineprint/Include/**.h", "Mineprint/Include/**.hpp", 
		"Mineprint/Source/**.cpp", "Mineprint/Source/**.c",
		"Content/**",
		"TODO.md", "README.md",
	}

	includedirs 
	{ 
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.SDL}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.stb}",

		"Mineprint/Include"
	}

	filter "system:windows"
		libdirs 
		{
			"Mineprint/Vendor/SDL/Lib/Win64"
			-- "Mineprint/Vendor/Steamworks/Lib/Win64",
			-- "Mineprint/Vendor/FMOD/Lib/Win64"
		}

	filter "system:linux"
		libdirs 
		{
			"Mineprint/Vendor/SDL/Lib/Linux64",
		}
		
	filter {}

	-- Linking order matters with gcc! freetype needs to be at the bottom.
	-- MW @todo: Understand linking order, consider using 'linkgroups "On"' to avoid linking order issues at the cost of link time.
	-- https://premake.github.io/docs/linkgroups/
	links
	{
		"imgui",
		"SDL3",

		-- MSDF
		-- "msdf-atlas-gen",
		-- "msdfgen",
		-- "freetype",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
	 	prebuildcommands { "call \"../Scripts/RunPreprocessor.bat\" \"../../../../../Build/%{prj.name}/" .. outputdir .. "/Content/\" \"%{cfg.buildcfg}\"" }

	filter "system:linux"
		prebuildcommands { "../Scripts/RunPreprocessor.sh \"../../../../../Build/%{prj.name}/" .. outputdir .. "/Content/\" \"%{cfg.buildcfg}\"" }

	filter { "system:linux", "files:Mineprint/Source/Vendor/stb.cpp" }
		optimize "On" -- MW @hack: stb doesn't compile properly with GCC without optimizations (@credit https://git.suyu.dev/suyu/suyu/pulls/63)

os.mkdir("Mineprint/Source")
os.mkdir("Mineprint/Include")

filter "configurations:Debug"
	defines { "MP_DEBUG", "MP_ENABLE_ASSERTS", "MP_GL_DEBUG" }
	symbols "On"
	runtime "Debug"

filter "configurations:Release"
	defines { "MP_RELEASE", "MP_ENABLE_ASSERTS", "MP_GL_DEBUG" }
	optimize "On"
	symbols "On"
	runtime "Release"

filter "configurations:Dist"
	defines { "MP_DIST", "MP_DISABLE_ASSERTS" }
	kind "WindowedApp"
	optimize "On"
	symbols "Off"
	runtime "Release"

filter "system:windows"
	systemversion "latest"
	defines { "MP_PLATFORM_WINDOWS" }

	links
	{
		"version",
		"winmm",
		"Imm32",
		"Cfgmgr32",
		"Setupapi"
	}

filter "system:linux"
	defines { "MP_PLATFORM_LINUX" }

filter "platforms:Win64"
	system "Windows"
	architecture "x64"

filter "platforms:Linux"
	buildoptions { "-static-libstdc++" }
	system "linux"
	architecture "x64"
