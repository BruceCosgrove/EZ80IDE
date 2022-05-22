#pragma once

#include <atomic>
#include <thread>

namespace ide::emu
{
	class EmulatorThread
	{
	public:
		~EmulatorThread();
	public:
		bool Load(const char* pROMPath);
		void Unload();
		void Start();
		void Stop();
	public:
		bool IsLoaded() const;
		bool IsRunning() const;
	private:
		void Run();
	private:
		std::atomic_bool m_Loaded = false;
		std::atomic_bool m_Running = false;
		std::thread m_Thread;
	};
}
