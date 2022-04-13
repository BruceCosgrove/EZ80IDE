GBC Requires the Vulkan SDK, so you can get the installer here:
https://sdk.lunarg.com/sdk/download/1.2.170.0/windows/vulkan_sdk.exe

Here are the debug libraries as well:
https://sdk.lunarg.com/sdk/download/1.2.170.0/windows/VulkanSDK-1.2.170.0-DebugLibs.zip

You only need these lib files from the debug libs ("Lib", not "Lib32"):
"shaderc_sharedd.lib", "spirv-cross-cored.lib", "spirv-cross-glsld.lib", "SPIRV-Toolsd.lib"
As well as the three dlls from "Bin" (again, not "Bin32"):
"shaderc_sharedd.dll", "spirv-cross-cored.dll", "SPIRV-Toolsd.dll"
To make this easy, go to the VULKAN_SDK directory (default is C:\VulkanSDK\1.2.170.0).
Create a new folder called "Debug" and two inside that one: "Lib" and "Bin".
Move those four lib files into the "Lib" folder and the three dlls into the "Bin" folder.
