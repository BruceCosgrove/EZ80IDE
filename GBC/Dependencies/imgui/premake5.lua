project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. OutputDir .. "/%{prj.name}")

	files {
		"include/imgui/**.h",
		"src/**.cpp"
	}

	includedirs {
		"include/imgui",
		"%{wks.location}/GBC/src",

		"%{IncludeDir.glad}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.spdlog}",

		"%{IncludeDir.glm}"
	}

	filter "system:windows"
		systemversion "latest"
		defines "GBC_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "GBC_CONFIG_DEBUG"
		runtime "Debug"
		optimize "Off"
		symbols "On"

	filter "configurations:Release"
		defines "GBC_CONFIG_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		defines "GBC_CONFIG_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"
