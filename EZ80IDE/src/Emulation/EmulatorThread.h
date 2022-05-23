#pragma once

#include "GBC/Rendering/LocalTexture2D.h"
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
		bool ShouldLCDUpdate();
	public:
		inline void SetLCDTexture(const gbc::Ref<gbc::LocalTexture2D>& lcdTexture) noexcept { m_LCDTexture = lcdTexture; }
	private:
		void Run();
		void Throttle();
	private:
		std::thread m_Thread;
		std::atomic_bool m_Loaded = false;
		std::atomic_bool m_Running = false;
		std::atomic_bool m_ShouldLCDUpdate = false;

		gbc::Ref<gbc::LocalTexture2D> m_LCDTexture;
	};
}
