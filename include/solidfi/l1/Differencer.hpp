// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Differencer.hpp
/// @ingroup solidfi_l1_state

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Delta.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_state
/// @reserved
/// @brief Given a current state T and a previous state T (as P), produces a Delta<T>.
///
/// Differencer is Converter<T, Delta<T>, T> — the previous state flows in as the
/// parameter P. The result is the named, first-class change between the two states.
///
/// @tparam T source type; free generic, owned by the user. Both the current and
///   previous state are of type T.
template<typename T>
class Differencer : public Converter<T, Delta<T>, T> {
public:
    /// @brief Produce the delta between value (current state) and previousState (prior state).
    ///
    /// @param value     The current state.
    /// @param previousState  The previous state, passed as the parameter P.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    Delta<T> resolve(T value, T previousState) override = 0;
};

} // namespace solidfi
