// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Sentinel.hpp
/// @ingroup solidfi_l0

namespace solidfi {

/// @ingroup solidfi_l0
/// @proposed
/// @brief Abstract concept: a value that signals absence or failure for type U.
///
/// Sentinel<U> is a Closed<U> with semantic meaning — it holds a fixed value of U
/// and returns it on demand via a static value() method. Specialize to declare which
/// value of U represents failure.
///
/// L1 currently has no dedicated failure type — Chain defines failure as a plain,
/// per-instance value of U (its `failed` member) rather than a named wrapper.
/// Sentinel<U> remains useful when U is a type that cannot be changed (third-party
/// or primitive types) and must use a reserved in-band value to signal failure.
///
/// The primary template is intentionally undefined. Using an unspecialized Sentinel<U>
/// is a compile error — a deliberate participation contract.
///
/// Example:
/// @code
///   template<>
///   struct Sentinel<int> {
///       static constexpr int value() { return -1; }
///   };
/// @endcode
///
/// @tparam U the type for which a failure sentinel is declared.
/// @note L0 mapping: Closed<T> — Sentinel<U> is a Closed<U> with semantic meaning.
/// @note Pattern: std::hash<T> — same non-intrusive specialization mechanism.
template<typename U>
struct Sentinel;

} // namespace solidfi
