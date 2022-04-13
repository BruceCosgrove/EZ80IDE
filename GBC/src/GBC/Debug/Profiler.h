#pragma once

#include "GBC/Core/Core.h"
#if GBC_ENABLE_PROFILE_RUNTIME || GBC_ENABLE_PROFILE
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

namespace gbc
{
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string name;

		FloatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession
	{
		InstrumentationSession(const std::string& name);

		std::string name;
	};

	class Profiler
	{
	public:
		Profiler() = default;
		Profiler(const Profiler&) = delete;
		Profiler(Profiler&&) = delete;
		static Profiler& Get();
		~Profiler();

		void BeginSession(const std::string& name, const std::filesystem::path& filepath = "results.json");
		void EndSession();
		void WriteProfile(const ProfileResult& result);
	private:
		void WriteHeader();
		void WriteFooter();

		// You must already own lock on mutex before calling this
		void InternalEndSession();
	private:
		std::mutex mutex;
		InstrumentationSession* currentSession = nullptr;
		std::ofstream outputStream;
	};

	class ProfilerTimer
	{
	public:
		ProfilerTimer(const char* name);
		~ProfilerTimer();

		void Stop();
	private:
		const char* name;
		std::chrono::time_point<std::chrono::steady_clock> startTimepoint;
		bool stopped;
	};
}
#endif

#if GBC_ENABLE_PROFILE_RUNTIME
	#define GBC_PROFILE_BEGIN_RUNTIME(name, filePath) ::gbc::Profiler::Get().BeginSession(name, filePath)
	#define GBC_PROFILE_END_RUNTIME() ::gbc::Profiler::Get().EndSession()
	#define GBC_PROFILE_BEGIN(name, filePath)
	#define GBC_PROFILE_END()
	#define GBC_PROFILE_SCOPE(name) ::gbc::ProfilerTimer timer##__LINE__(name)
	#define GBC_PROFILE_FUNCTION() GBC_PROFILE_SCOPE(__FUNCSIG__)
#elif GBC_ENABLE_PROFILE
	#define GBC_PROFILE_BEGIN_RUNTIME(name, filePath)
	#define GBC_PROFILE_END_RUNTIME()
	#define GBC_PROFILE_BEGIN(name, filePath) ::gbc::Profiler::Get().BeginSession(name, filePath)
	#define GBC_PROFILE_END() ::gbc::Profiler::Get().EndSession()
	#define GBC_PROFILE_SCOPE(name) ::gbc::ProfilerTimer timer##__LINE__(name)
	#define GBC_PROFILE_FUNCTION() GBC_PROFILE_SCOPE(__FUNCSIG__)
#else
	#define GBC_PROFILE_BEGIN_RUNTIME(name, filePath)
	#define GBC_PROFILE_END_RUNTIME()
	#define GBC_PROFILE_BEGIN(name, filePath)
	#define GBC_PROFILE_END()
	#define GBC_PROFILE_SCOPE(name)
	#define GBC_PROFILE_FUNCTION()
#endif
