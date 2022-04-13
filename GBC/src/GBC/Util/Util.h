#pragma once

#include <string>

namespace gbc::util
{
	// Converts an integral x in the given radix(aka base) to a string, either using uppercase or lowercase letters.
	// Radix is in the range [2, 37), anything outside this range will be considered invalid.
	// Returns if the conversion was successful.
	template<typename Integral, class Elem = char, class Traits = std::char_traits<Elem>, class Alloc = std::allocator<Elem>>
	bool IToS(Integral integral, std::basic_string<Elem, Traits, Alloc>& outString, uint8_t radix = 10, bool lowercase = false);

	// Converts a string to an integral in the given radix(aka base).
	// Prepending and apending whitespace count as invalid.
	// Radix is in the range [2, 37), anything outside this range will be considered invalid.
	// Returns if the conversion was successful. If not, outIntegral may or may not modified.
	template<typename Integral, class Elem = char, class Traits = std::char_traits<Elem>>
	bool SToI(std::basic_string_view<Elem, Traits> stringView, Integral& outIntegral, uint8_t radix = 10);

	// Alias for bool SToI(std::basic_string_view<Elem, Traits> stringView, Integral& outIntegral, uint8_t radix).
	// Because of templates, the implicit std::basic_string_view cast doesn't happen, so this is doing that manually.
	template<typename Integral, class Elem = char, class Traits = std::char_traits<Elem>, class Alloc = std::allocator<Elem>>
	inline bool SToI(const std::basic_string<Elem, Traits, Alloc>& string, Integral& outIntegral, uint8_t radix = 10)
	{ return SToI((std::basic_string_view<Elem, Traits>)string, outIntegral, radix); }
}

#include "Util.inl"
