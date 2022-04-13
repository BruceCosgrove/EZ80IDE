#include "gbcpch.h"
#include "OpenGLShader.h"
#include "GBC/IO/FileIO.h"
#include "GBC/Util/Timer.h"
#include <glad/glad.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace gbc
{
	static constexpr GLenum StringToGLShaderStage(std::string_view type)
	{
		if (type == "vertex")                 return GL_VERTEX_SHADER;
		if (type == "fragment")               return GL_FRAGMENT_SHADER;
		if (type == "geometry")               return GL_GEOMETRY_SHADER;
		if (type == "compute")                return GL_COMPUTE_SHADER;
		if (type == "tessolation control")    return GL_TESS_CONTROL_SHADER;
		if (type == "tessolation evaluation") return GL_TESS_EVALUATION_SHADER;

		GBC_CORE_ASSERT(false, "Unknown Shader Type!");
		return 0;
	}

	static constexpr const char* GLShaderStageToString(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:          return "vertex";
			case GL_FRAGMENT_SHADER:        return "fragment";
			case GL_GEOMETRY_SHADER:        return "geometry";
			case GL_COMPUTE_SHADER:         return "compute";
			case GL_TESS_CONTROL_SHADER:    return "tessolation control";
			case GL_TESS_EVALUATION_SHADER: return "tessolation evaluation";
		}

		GBC_CORE_ASSERT(false, "Unknown Shader Stage!");
		return nullptr;
	}

	static constexpr shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER:          return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER:        return shaderc_glsl_fragment_shader;
			case GL_GEOMETRY_SHADER:        return shaderc_glsl_geometry_shader;
			case GL_COMPUTE_SHADER:         return shaderc_glsl_compute_shader;
			case GL_TESS_CONTROL_SHADER:    return shaderc_glsl_tess_control_shader;
			case GL_TESS_EVALUATION_SHADER: return shaderc_glsl_tess_evaluation_shader;
		}

		GBC_CORE_ASSERT(false, "Unknown Shader Stage!");
		return static_cast<shaderc_shader_kind>(0);
	}

	static const char* GetCacheDirectory()
	{
		return "Resources/Cache/Shaders/OpenGL";
	}

	static void CreateCacheDirectoryIfNeeded()
	{
		std::filesystem::path directoryPath = GetCacheDirectory();
		if (!std::filesystem::exists(directoryPath))
			std::filesystem::create_directories(directoryPath);
	}

	static constexpr const char* GetShaderStageCachedVulkanFileExtensions(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER: return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER: return ".cached_vulkan.frag";
		}

		GBC_CORE_ASSERT(false, "Unknown shader stage for vulkan cache name!");
		return nullptr;
	}

	static constexpr const char* GetShaderStageCachedOpenGLFileExtensions(uint32_t stage)
	{
		switch (stage)
		{
			case GL_VERTEX_SHADER: return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
		}

		GBC_CORE_ASSERT(false, "Unknown shader stage for opengl cache name!");
		return nullptr;
	}

	OpenGLShader::OpenGLShader(const std::filesystem::path& filepath)
		: filepath(filepath)
	{
		GBC_PROFILE_FUNCTION();

		CreateCacheDirectoryIfNeeded();

		if (auto file = FileIO::ReadFile(filepath); !file.empty())
		{
			Timer timer;

			auto shaderSources = PreProcess(file);
			CompileOrGetVulkanBinaries(shaderSources);

			// Get shader information
			for (auto&& [stage, data] : vulkanSPIRV)
				Reflect(stage, data);

			CompileOrGetOpenGLBinaries();
			CreateProgram();

			GBC_CORE_WARN("Shader creation took {0}ms.", timer.GetElapsedTime().Millis());
		}
		else
			GBC_CORE_ERROR("Shader file does not exist or is empty!");
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(rendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(rendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	std::unordered_map<uint32_t, std::string> OpenGLShader::PreProcess(const std::string& sourceCode)
	{
		std::unordered_map<uint32_t, std::string> shaders;

		static constexpr char typeToken[] = "#type";
		static const size_t typeTokenLength = sizeof(typeToken) / sizeof(*typeToken);
		size_t position = sourceCode.find(typeToken, 0);

		while (position != std::string::npos)
		{
			size_t lineEnd = sourceCode.find_first_of("\r\n", position);
			GBC_CORE_ASSERT(lineEnd != std::string::npos, "Cannot have #type at the end of the file!");

			size_t typeStart = position + typeTokenLength;
			std::string_view typeString = std::string_view(sourceCode).substr(typeStart, lineEnd - typeStart);
			GLenum type = StringToGLShaderStage(typeString);
			GBC_CORE_ASSERT(shaders.find(type) == shaders.end(), "Cannot add the same shader type twice!");

			size_t nextLineStart = sourceCode.find_first_not_of("\r\n", lineEnd);
			GBC_CORE_ASSERT(nextLineStart != std::string::npos, "Must include code for a shader!");
			position = sourceCode.find(typeToken, nextLineStart);
			size_t nextLineEnd = position == std::string::npos ? std::string::npos : position - nextLineStart;
			shaders[type] = sourceCode.substr(nextLineStart, nextLineEnd);
		}

		return shaders;
	}

	void OpenGLShader::CompileOrGetVulkanBinaries(const std::unordered_map<uint32_t, std::string>& shaderSources)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		options.AddMacroDefinition("OPENGL", "1");
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		vulkanSPIRV.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GetShaderStageCachedVulkanFileExtensions(stage));

			// Use the cache if it exists and is up to date, otherwise, (re)compile the shader.
			bool useCacheFile = false;
			if (std::filesystem::exists(cachedPath))
			{
				auto sourceModTime = std::filesystem::last_write_time(shaderFilePath);
				auto cacheModTime = std::filesystem::last_write_time(cachedPath);
				bool sourceUpdated = sourceModTime > cacheModTime;
				useCacheFile = !sourceUpdated;
			}

			if (useCacheFile)
			{
				std::ifstream cachedFile(cachedPath, std::ios::in | std::ios::binary);
				GBC_ASSERT(cachedFile.is_open(), "Could not open shader cache file!");

				cachedFile.seekg(0, std::ios::end);
				auto size = cachedFile.tellg();
				cachedFile.seekg(0, std::ios::beg);

				auto& data = vulkanSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				cachedFile.read((char*)data.data(), size);
				cachedFile.close();
			}
			else
			{
				auto filepathString = filepath.string();
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), filepathString.c_str(), options);
#if GBC_ENABLE_ASSERTS
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					GBC_CORE_ASSERT(false, module.GetErrorMessage());
#endif

				vulkanSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream cachedFileOut(cachedPath, std::ios::out | std::ios::binary);
				if (cachedFileOut.is_open())
				{
					auto& data = vulkanSPIRV[stage];
					cachedFileOut.write((char*)data.data(), data.size() * sizeof(uint32_t));
					cachedFileOut.flush();
					cachedFileOut.close();
				}
			}
		}
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries()
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = GetCacheDirectory();

		openglSPIRV.clear();
		openglSourceCode.clear();
		for (auto&& [stage, spirv] : vulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = filepath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + GetShaderStageCachedOpenGLFileExtensions(stage));

			// Use the cache if it exists and is up to date, otherwise, (re)compile the shader.
			bool useCacheFile = false;
			if (std::filesystem::exists(cachedPath))
			{
				auto sourceModTime = std::filesystem::last_write_time(shaderFilePath);
				auto cacheModTime = std::filesystem::last_write_time(cachedPath);
				bool sourceUpdated = sourceModTime > cacheModTime;
				useCacheFile = !sourceUpdated;
			}

			if (useCacheFile)
			{
				std::ifstream cachedFile(cachedPath, std::ios::in | std::ios::binary);
				GBC_ASSERT(cachedFile.is_open(), "Could not open shader cache file!");

				cachedFile.seekg(0, std::ios::end);
				auto size = cachedFile.tellg();
				cachedFile.seekg(0, std::ios::beg);

				auto& data = openglSPIRV[stage];
				data.resize(size / sizeof(uint32_t));
				cachedFile.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompiler(spirv);
				openglSourceCode[stage] = glslCompiler.compile();
				auto& source = openglSourceCode[stage];

				auto filepathString = filepath.string();
				shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), filepathString.c_str(), options);
#if GBC_ENABLE_ASSERTS
				if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					GBC_CORE_ASSERT(false, module.GetErrorMessage());
#endif

				openglSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

				std::ofstream cachedFileOut(cachedPath, std::ios::out | std::ios::binary);
				if (cachedFileOut.is_open())
				{
					auto& data = openglSPIRV[stage];
					cachedFileOut.write((char*)data.data(), data.size() * sizeof(uint32_t));
					cachedFileOut.flush();
					cachedFileOut.close();
				}
			}
		}
	}

	void OpenGLShader::Reflect(uint32_t stage, const std::vector<uint32_t>& data)
	{
		spirv_cross::Compiler compiler(data);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		auto filepathString = filepath.string();
		GBC_CORE_TRACE("Shader Compilation Info for {0} shader in \"{1}\".", GLShaderStageToString(stage), filepathString);

		GBC_CORE_TRACE("  {0} uniform buffer{1}", resources.uniform_buffers.size(), resources.uniform_buffers.size() != 1 ? "s" : "");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			size_t size = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			size_t memberCount = bufferType.member_types.size();

			GBC_CORE_TRACE("    {0}", resource.name);
			GBC_CORE_TRACE("      Size = {0}", size);
			GBC_CORE_TRACE("      Binding = {0}", binding);
			GBC_CORE_TRACE("      Members = {0}", memberCount);
		}

		GBC_CORE_TRACE("  {0} resource{1}", resources.sampled_images.size(), resources.sampled_images.size() != 1 ? "s" : "");
	}

	void OpenGLShader::CreateProgram()
	{
		rendererID = glCreateProgram();

		std::vector<RendererID> shaderIDs;
		for (auto&& [stage, spirv] : openglSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), static_cast<GLsizei>(spirv.size() * sizeof(uint32_t)));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(rendererID, shaderID);
		}
	
		bool success = LinkAndValidate();
	
		// Do this regardles of success
		for (auto shaderID : shaderIDs)
		{
			glDetachShader(rendererID, shaderID);
			glDeleteShader(shaderID);
		}

#if GBC_CONFIG_DEBUG
		bool clear = false;
#else
		bool clear = true;
#endif

		if (!success)
		{
			GBC_CORE_WARN("Failed to create shader.");
			glDeleteProgram(rendererID);
			rendererID = 0;
			filepath.clear();
			clear = true;
		}

		if (clear)
		{
			vulkanSPIRV.clear();
			openglSPIRV.clear();
			openglSourceCode.clear();
		}
	}

	bool OpenGLShader::LinkAndValidate()
	{
		// Link
		glLinkProgram(rendererID);
		GLint link;
		glGetProgramiv(rendererID, GL_LINK_STATUS, &link);
		if (link == GL_FALSE)
		{
			GLint length;
			glGetProgramiv(rendererID, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> message(length);
			glGetProgramInfoLog(rendererID, length, &length, message.data());
			GBC_CORE_WARN("{0}", static_cast<char*>(message.data()));
			return false;
		}

		// Validate
		glValidateProgram(rendererID);
		GLint validate;
		glGetProgramiv(rendererID, GL_VALIDATE_STATUS, &validate);
		if (validate == GL_FALSE)
		{
			GLint length;
			glGetProgramiv(rendererID, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> message(length);
			glGetProgramInfoLog(rendererID, length, &length, message.data());
			GBC_CORE_WARN("{0}", static_cast<char*>(message.data()));
			return false;
		}

		return true;
	}

	int32_t OpenGLShader::GetUniformLocation(const std::string& name)
	{
		auto it = uniformLocations.find(name);
		if (it != uniformLocations.end())
			return it->second;

		int32_t location = glGetUniformLocation(rendererID, name.c_str());
#if GBC_ENABLE_LOGGING // to remove warning of empty if statement
		if (location == -1)
			GBC_CORE_WARN("Unused shader uniform: {0}", name);
#endif
		uniformLocations[name] = location;
		return location;
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1f(location, value);
	}

	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform2fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform4fv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetInt(const std::string& name, int32_t value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1i(location, value);
	}

	void OpenGLShader::SetInt2(const std::string& name, const glm::ivec2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform2iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetInt3(const std::string& name, const glm::ivec3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform3iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetInt4(const std::string& name, const glm::ivec4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform4iv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUInt(const std::string& name, uint32_t value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1ui(location, value);
	}

	void OpenGLShader::SetUInt2(const std::string& name, const glm::uvec2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform2uiv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUInt3(const std::string& name, const glm::uvec3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform3uiv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetUInt4(const std::string& name, const glm::uvec4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform4uiv(location, 1, glm::value_ptr(value));
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1i(location, (int)value);
	}

	void OpenGLShader::SetBool2(const std::string& name, const glm::bvec2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform2i(location, (int)value.x, (int)value.y);
	}

	void OpenGLShader::SetBool3(const std::string& name, const glm::bvec3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform3i(location, (int)value.x, (int)value.y, (int)value.z);
	}

	void OpenGLShader::SetBool4(const std::string& name, const glm::bvec4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform4i(location, (int)value.x, (int)value.y, (int)value.z, (int)value.w);
	}

	void OpenGLShader::SetMat2(const std::string& name, const glm::mat2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat2x3(const std::string& name, const glm::mat2x3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat2x4(const std::string& name, const glm::mat2x4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat3x2(const std::string& name, const glm::mat3x2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat3x4(const std::string& name, const glm::mat3x4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat4x2(const std::string& name, const glm::mat4x2& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat4x3(const std::string& name, const glm::mat4x3& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void OpenGLShader::SetInts(const std::string& name, const int32_t* values, int32_t count)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1iv(location, count, values);
	}

	void OpenGLShader::SetUInts(const std::string& name, const uint32_t* values, int32_t count)
	{
		int32_t location = GetUniformLocation(name);
		if (location != -1) glUniform1uiv(location, count, values);
	}
}
