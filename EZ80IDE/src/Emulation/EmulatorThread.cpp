#include "EmulatorThread.h"
#include "GBC/Core/Core.h"
#include "core/cpu.h"
#include "core/cemu.h"
#include "core/debug/debug.h"

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
		emu_state_t state = emu_load(EMU_DATA_ROM, pROMPath);
		return state == EMU_STATE_VALID;
	}

	void EmulatorThread::Unload()
	{
		if (m_Loaded)
		{
			m_Loaded = false;
			Stop();
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

	void EmulatorThread::Run()
	{
		while (m_Running && cpu.abort != CPU_ABORT_EXIT)
		{
			emu_run(1);
			// TODO: user input
			// TODO: throttle emulation speed
		}

		asic_free();
	}
}
