#pragma once

/// @file Applicator.hpp
/// @ingroup solidfi_l1_state

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Delta.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_state
/// @brief Given a current state T and a Delta<T> (as P), produces the next state T.
///
/// Applicator is Converter<T, T, Delta<T>> — the delta flows in as the parameter P.
/// The result is the new state after applying the change.
///
/// @tparam T source type; free generic, owned by the user. Both input and output are T.
template<typename T>
class Applicator : public Converter<T, T, Delta<T>> {
public:
    /// @brief Apply the delta to the current state to produce the next state.
    ///
    /// @param value  The current state.
    /// @param delta  The change to apply, passed as the parameter P.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    T fetch(T value, Delta<T> delta) override = 0;
};

} // namespace solidfi
