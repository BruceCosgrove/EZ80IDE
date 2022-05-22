include "vendor/premake/solution_items.lua"
include "Dependencies.lua"

workspace "EZ80IDE"
	architecture "x86_64"
	startproject "EZ80IDE"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	solution_items {
		"premake5.lua",
		"Dependencies.lua",
		".editorconfig",
		".gitignore",
		".gitattributes"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies/GBC"
	include "GBC/vendor/glad"
	include "GBC/vendor/glfw"
	include "GBC/vendor/imgui"
	include "GBC/vendor/stb"
group "Dependencies/EZ80IDE"
	include "EZ80IDE/vendor/CEmu"
group ""

include "GBC"
include "EZ80IDE"
