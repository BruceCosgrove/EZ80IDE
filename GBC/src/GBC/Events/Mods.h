#pragma once

#include <cstdint>

namespace gbc
{
	enum : uint8_t
	{
		Mods_None     = 0,
		Mods_Control  = 1 << 0,
		Mods_Shift    = 1 << 1,
		Mods_Alt      = 1 << 2,
		Mods_Super    = 1 << 3,
		Mods_CapsLock = 1 << 4,
		Mods_NumLock  = 1 << 5,

		// Automatically creates the mods mask
		Mods_Last, Mods_All = (Mods_Last << 1) - 3
	};

	class Mods
	{
	public:
		constexpr Mods() noexcept = default;
		constexpr Mods(uint8_t mods) noexcept : mods(Mask(mods)) {}

		constexpr bool Any(uint8_t mods) const noexcept { return !!(this->mods & mods); }
		constexpr bool Has(uint8_t mods) const noexcept { return (this->mods & mods) == mods; }
		constexpr bool Are(uint8_t mods) const noexcept { return this->mods == mods; }

		constexpr operator uint8_t() const noexcept { return mods; }

		constexpr bool None() const noexcept { return mods == Mods_None; }
		constexpr bool operator!() const noexcept { return None(); }
		explicit constexpr operator bool() const noexcept { return !None(); }
	private:
		static constexpr uint8_t Mask(uint8_t mods) noexcept { return mods & Mods_All; }
	private:
		uint8_t mods = Mods_None;
	};
}
