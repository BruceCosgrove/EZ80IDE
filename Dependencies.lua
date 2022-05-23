VULKAN_SDK = os.getenv("VULKAN_SDK")

includedir = {}
includedir["glad"] = "%{wks.location}/GBC/vendor/glad/include"
includedir["glfw"] = "%{wks.location}/GBC/vendor/glfw/include"
includedir["imgui"] = "%{wks.location}/GBC/vendor/imgui/include"
includedir["stb"] = "%{wks.location}/GBC/vendor/stb/include"
includedir["yaml"] = "%{wks.location}/GBC/vendor/yaml/include"
includedir["CEmu"] = "%{wks.location}/EZ80IDE/vendor/CEmu"
-- Header-only; they don't have their own visual studio project
includedir["glm"] = "%{wks.location}/GBC/vendor/glm/include"
includedir["spdlog"] = "%{wks.location}/GBC/vendor/spdlog/include"
-- Header+Lib; they also don't have their own visual studio project
includedir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

librarydir = {}

librarydir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
librarydir["VulkanSDK_Debug"] = "%{VULKAN_SDK}/Debug/Lib"
librarydir["VulkanSDK_DebugDLL"] = "%{VULKAN_SDK}/Debug/Bin"

library = {}
library["Vulkan"] = "%{librarydir.VulkanSDK}/vulkan-1.lib"
library["VulkanUtils"] = "%{librarydir.VulkanSDK}/VkLayer_utils.lib"

library["ShaderC_Debug"] = "%{librarydir.VulkanSDK_Debug}/shaderc_sharedd.lib"
library["SPIRV_Cross_Debug"] = "%{librarydir.VulkanSDK_Debug}/spirv-cross-cored.lib"
library["SPIRV_Cross_GLSL_Debug"] = "%{librarydir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
library["SPIRV_Tools_Debug"] = "%{librarydir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

library["ShaderC_Release"] = "%{librarydir.VulkanSDK}/shaderc_shared.lib"
library["SPIRV_Cross_Release"] = "%{librarydir.VulkanSDK}/spirv-cross-core.lib"
library["SPIRV_Cross_GLSL_Release"] = "%{librarydir.VulkanSDK}/spirv-cross-glsl.lib"
