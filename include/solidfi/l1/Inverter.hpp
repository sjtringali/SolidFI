// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Inverter.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @proposed
/// @brief A pair of converters that roundtrip T and U, implementing both interfaces.
///
/// Inverter implements both Converter<T,U> and Converter<U,T> directly. The Composite/
/// Liskov rule extends here: an Inverter can be swapped in anywhere either interface is
/// expected, for either direction independently.
///
/// forward() and reverse() are the primary named entry points for each direction. resolve()
/// is implemented on both interfaces as a forwarding alias — like dispatch() on Chain or
/// run() on Pipeline, it exists to satisfy the Converter contract; prefer forward()/reverse()
/// when calling a known Inverter directly.
///
/// @code
///   Inverter<Dude, Lady> aerosmith;
///   Dude dude = ...;
///   Lady lady = aerosmith.forward(dude);   // T -> U
///   Dude back = aerosmith.reverse(lady);   // U -> T
///   auto surprise = aerosmith.resolve(dude);
/// @endcode
///
/// This bidirectionality is the basis for L2 Roundtrip.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
template<typename T, typename U>
class Inverter : public Converter<T, U>, public Converter<U, T> {
public:
    /// @brief Convert in the forward direction: T to U. Primary entry point.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    virtual U forward(T value, Parameters params) = 0;

    /// @brief Convert in the reverse direction: U to T. Primary entry point.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    virtual T reverse(U value, Parameters params) = 0;

    /// @brief Forwarding alias satisfying Converter<T,U>. Prefer forward() when calling directly.
    U resolve(T value, Parameters params) override { return forward(value, params); }

    /// @brief Forwarding alias satisfying Converter<U,T>. Prefer reverse() when calling directly.
    T resolve(U value, Parameters params) override { return reverse(value, params); }
};

} // namespace solidfi
