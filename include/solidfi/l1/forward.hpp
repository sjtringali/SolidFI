#pragma once

/// @file forward.hpp
/// @brief SolidFI L1 — implementation infrastructure. Not named L1 concepts.
///
/// Provides the building blocks needed for L1 headers to compile:
/// - NoInput: tag type for Generator<T> (Converter<void,T> workaround)

namespace solidfi {

/// @brief Tag type representing the absence of a meaningful input.
///
/// Used as the input type for Generator<T>, which conceptually is Converter<void,T,P>.
/// C++ does not permit void as a template type argument, so NoInput serves as the
/// equivalent empty placeholder.
struct NoInput {};

} // namespace solidfi
