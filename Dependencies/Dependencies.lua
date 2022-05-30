VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
LibraryDir = {}
Library = {}

-- Include Directories
	-- Header and Source; they have their own project.
	IncludeDir["glad"] = "%{wks.location}/GBC/Dependencies/glad/include"
	IncludeDir["glfw"] = "%{wks.location}/GBC/Dependencies/glfw/include"
	IncludeDir["imgui"] = "%{wks.location}/GBC/Dependencies/imgui/include"
	IncludeDir["stb"] = "%{wks.location}/GBC/Dependencies/stb/include"
	IncludeDir["yaml"] = "%{wks.location}/GBC/Dependencies/yaml/include"
	IncludeDir["CEmu"] = "%{wks.location}/EZ80IDE/Dependencies/CEmu"

	-- Header-Only; they don't have their own project.
	IncludeDir["glm"] = "%{wks.location}/GBC/Dependencies/glm/include"
	IncludeDir["spdlog"] = "%{wks.location}/GBC/Dependencies/spdlog/include"

	-- Header and Lib; they also don't have their own project.
	IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

-- Library Directories
	-- VulkanSDK
	LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
	LibraryDir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Debug/Lib"
	LibraryDir["VulkanSDK_DebugDLL"] = "%{VULKAN_SDK}/Debug/Bin"

-- Libraries
	-- Vulkan
	Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
	Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

	-- ShaderC Debug
	Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
	Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
	Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
	Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

	-- ShaderC Release
	Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
	Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
	Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
