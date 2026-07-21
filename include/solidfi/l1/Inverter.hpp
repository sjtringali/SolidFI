// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Inverter.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @reserved
/// @brief A Converter that guarantees bidirectional conversion between T and U.
///
/// An Inverter can convert T to U (forward) and U to T (reverse). The bidirectionality
/// guarantee distinguishes it from a plain Converter: callers can rely on the reverse
/// direction being available.
///
/// This bidirectionality is the basis for L2 Roundtrip.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
///
/// @todo Full specification pending. Reverse resolve signature (parameters, failure semantics)
///   and composition rules for inverted chains are TBD.
template<typename T, typename U>
class Inverter : public Converter<T, U> {
public:
    /// @brief Convert in the forward direction: T to U.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, Parameters params) override = 0;

    /// @brief Convert in the reverse direction: U to T.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    virtual T invert(U value, Parameters params) = 0;
};

} // namespace solidfi
