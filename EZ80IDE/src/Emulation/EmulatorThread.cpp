#include "EmulatorThread.h"
#include "GBC/Core/Core.h"
#include "CEmu/core/cpu.h"
#include "CEmu/core/cemu.h"
#include "CEmu/core/debug/debug.h"
#include <chrono>

namespace ide::emu
{
	EmulatorThread::~EmulatorThread()
	{
		Unload();
	}

	bool EmulatorThread::Load(const char* pROMPath)
	{
		Unload();
		m_Loaded = true;
#ifdef DEBUG_SUPPORT
		debug_init();
#endif
		return emu_load(EMU_DATA_ROM, pROMPath) == EMU_STATE_VALID;
	}

	void EmulatorThread::Unload()
	{
		if (m_Loaded)
		{
			m_Loaded = false;
			Stop();
			asic_free();
#ifdef DEBUG_SUPPORT
			debug_free();
#endif
		}
	}

	void EmulatorThread::Start()
	{
		if (!m_Running)
		{
			m_Running = true;
			m_Thread = std::thread(GBC_BIND_FUNC(Run));
		}
	}

	void EmulatorThread::Stop()
	{
		if (m_Running)
		{
			m_Running = false;
			m_ShouldLCDUpdate = false;
			m_Thread.join();
		}
	}

	bool EmulatorThread::IsLoaded() const
	{
		return m_Loaded;
	}

	bool EmulatorThread::IsRunning() const
	{
		return m_Running;
	}

	bool EmulatorThread::ShouldLCDUpdate()
	{
		bool shouldLCDUpdate = m_ShouldLCDUpdate;
		if (!shouldLCDUpdate)
			return false;
		m_ShouldLCDUpdate = false;
		return shouldLCDUpdate;
	}

	void EmulatorThread::Run()
	{
		while (m_Running && cpu.abort != CPU_ABORT_EXIT)
		{
			emu_run(1);

			// TODO: only draw frame and set m_ShouldLCDUpdate to true
			// if the lcd has actually updated, not after every tick.
			GBC_ASSERT(m_LCDTexture && *m_LCDTexture, "LCD Texture is nullptr in Emulator Thread.");
			emu_lcd_drawframe(m_LCDTexture->GetData());
			m_ShouldLCDUpdate = true;

			// TODO: user input

			Throttle();
		}

		Stop();
	}

	void EmulatorThread::Throttle()
	{
		static auto interval = std::chrono::duration_cast<std::chrono::system_clock::duration>(
			std::chrono::duration<std::chrono::system_clock::rep, std::ratio<1, 75/* is this the monitor refresh rate!?!?? */>>(1)
		);
		static auto lastTime = std::chrono::system_clock::now();
		
		auto currentTime = std::chrono::system_clock::now();
		auto nextTime = currentTime + interval;

		if (currentTime < nextTime)
		{
			lastTime = nextTime;
			std::this_thread::sleep_until(nextTime);
		}
		else
		{
			lastTime = currentTime;
			std::this_thread::yield();
		}
	}
}
