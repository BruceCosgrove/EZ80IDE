project "EZ80IDE"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	cdialect "C17"
	staticruntime "Off"

	targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. OutputDir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.c",
		"src/**.hpp",
		"src/**.cpp",
		"src/**.inl"
	}

	defines {
		"MULTITHREAD" -- For CEmu.
	}

	includedirs {
		"src",
		"%{wks.location}/GBC/src",

		"%{IncludeDir.imgui}",
		"%{IncludeDir.stb}",

		"%{IncludeDir.glm}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.yaml}",

		"%{IncludeDir.CEmu}"
	}

	links {
		"GBC",
		"CEmu"
	}

	filter "system:windows"
		systemversion "latest"
		defines "GBC_PLATFORM_WINDOWS"
-- Until Microsoft updates Windows 10 to not have terrible code, this must be here to prevent a warning
		buildoptions "/wd5105"

	filter "configurations:Debug"
		defines {
			"GBC_CONFIG_DEBUG",
			"DEBUG_SUPPORT" -- For CEmu.
		}
		runtime "Debug"
		optimize "Off"
		symbols "On"

		postbuildcommands {
			"pushd \"%{LibraryDir.VulkanSDK_DebugDLL}\"",
			"copy *.dll \"%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}\"",
			"popd"
		}

	filter "configurations:Release"
		defines "GBC_CONFIG_RELEASE"
		runtime "Release"
		optimize "On"
		symbols "On"

		excludes "src/Panels/Debug/*.cpp"

	filter "configurations:Dist"
		kind "WindowedApp"
		defines "GBC_CONFIG_DIST"
		runtime "Release"
		optimize "On"
		symbols "Off"

		excludes {
			"src/Panels/Debug/*.cpp",
			"src/Panels/Release/*.cpp"
		}
