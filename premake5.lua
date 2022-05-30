include "Dependencies/premake/Custom/solutionitems.lua"

workspace "EZ80IDE"
	architecture "x86_64"
	startproject "EZ80IDE"

	configurations { "Debug", "Release", "Dist" }

	solutionitems {
		-- Visual Studio
		".editorconfig",

		-- Git
		".gitignore",
		".gitattributes",

		-- Scripts
		"Scripts/GenerateProjects.bat",

		-- Lua Scripts
		"premake5.lua",
		"Dependencies/Dependencies.lua",
		"Dependencies/premake/Custom/solutionitems.lua",
		"Dependencies/premake/Custom/usestdpreproc.lua",
		
		-- Misc
		"README.md"
	}

	flags {
		"MultiProcessorCompile"
	}

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Dependencies/premake/Custom/usestdpreproc.lua"
include "Dependencies/Dependencies.lua"

group "Dependencies/GBC"
	include "GBC/Dependencies/glad"
	include "GBC/Dependencies/glfw"
	include "GBC/Dependencies/imgui"
	include "GBC/Dependencies/stb"
	include "GBC/Dependencies/yaml"
group "Dependencies/EZ80IDE"
	include "EZ80IDE/Dependencies/CEmu"
group ""

include "GBC"
include "EZ80IDE"
