// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file forward.hpp
/// @brief SolidFI L1 — implementation infrastructure. Not named L1 concepts.
///
/// Common types shared across L1 headers, collected here instead of one-per-file:
/// - Void: tag type for Generator<T> (Converter<void,T> workaround)
/// - Priority: ordering key for Pipeline::install and Chain::install
/// - Optional<T>: L1 concretization of L0's Optional<T>

#include <optional>

namespace solidfi {

/// @brief Tag type representing the absence of a meaningful input.
///
/// Used as the input type for Generator<T>, which conceptually is Converter<void,T,P>.
/// C++ does not permit void as a template type argument, so Void serves as the
/// equivalent empty placeholder.
struct Void {};

/// @brief Ordering key for entries installed into a Pipeline or Chain.
///
/// Lower values run first. In C++ this is an alias for int; in TypeScript this maps to
/// `number`. Named so the concept has an identity in the spec rather than a bare int.
using Priority = int;

/// @brief May or may not hold a value of type T.
///
/// L1 concretization of L0's Optional<T> — a direct alias for std::optional<T>. In
/// TypeScript this maps to `T | undefined`.
///
/// @tparam T source type; free generic, owned by the user.
/// @note L0 mapping: Optional<T> — named at L0 as the abstract concept; concretized here.
template<typename T>
using Optional = std::optional<T>;

} // namespace solidfi
