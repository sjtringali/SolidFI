// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Failed.hpp
/// @ingroup solidfi_l1_structural


namespace solidfi {

/// @ingroup solidfi_l1_structural

/// @accepted
/// @brief Explicit failure signal returned by a Converter to indicate fetch() did not succeed.
///
/// Failed<T> is a type-distinct failure signal — separate from T's value space entirely.
/// A Converter returns Failed<T> to tell Chain "I could not handle this; try the next one."
/// Chain tests for Failed<T> rather than comparing the result against a magic sentinel value,
/// which avoids consuming null, zero, or any other valid T value as a failure indicator.
///
/// Failed<T> is non-intrusive: T does not need to reserve a special value for failure.
/// Any T can participate as a Converter output type in a Chain without modification.
///
/// @tparam T the type whose failure is being signaled.
/// @note L0 mapping: Sentinel<T> — the abstract concept of a value that signals absence.
template<typename T>
struct Failed;

} // namespace solidfi
