#pragma once

/// @file forward.hpp
/// @brief SolidFI L1 — implementation infrastructure. Not named L1 concepts.
///
/// Provides the building blocks needed for L1 headers to compile:
/// - Optional<T>: L0 concept aliased to std::optional<T>
/// - NoInput: tag type for Generator<T> (Converter<void,T> workaround)

#include <optional>

namespace solidfi {

/// @brief May or may not hold a value of type T. L1 alias for std::optional<T>.
///
/// L0 names this concept; L1 concretizes it here. All Converter::fetch return types
/// use `Optional<U>` to represent possible failure.
///
/// @tparam T source type; free generic, owned by the user.
/// @note L0 mapping: Optional<T>
template<typename T>
using Optional = std::optional<T>;

/// @brief Tag type representing the absence of a meaningful input.
///
/// Used as the input type for Generator<T>, which conceptually is Converter<void,T,P>.
/// C++ does not permit void as a template type argument, so NoInput serves as the
/// equivalent empty placeholder.
struct NoInput {};

} // namespace solidfi
