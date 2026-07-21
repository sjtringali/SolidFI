// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Literal.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Transform.hpp"
#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @proposed
/// @brief Captures a value of type T and produces it regardless of input.
///
/// Literal simultaneously satisfies both Transform<T> and Converter<InputT,T> — it is
/// injectable wherever either is expected without the caller needing to choose.
///
/// Also known as Closed<T> at L0.
///
/// Common uses:
/// - Inject a constant into a Chain or Pipeline without changing the type contract.
/// - Adapt an existing value into a composition that expects a transform or converter.
/// - Stand-in during development before a real implementation exists.
///
/// **Invariants:**
/// - MUST always produce a T.
/// - resolve() and apply() MUST NOT depend on the input value.
/// - accepts() returns true, rejects() returns false.
///
/// @tparam T The captured and produced type; free generic, owned by the user.
/// @tparam InputT The input type for the Converter interface. Defaults to T.
/// @note L0 mapping: Closed<T>
template<typename T, typename InputT = T, typename P = Parameters>
class Literal : public Transform<T, P>, public Converter<InputT, T, P> {
public:
    /// @brief Capture an initial value.
    explicit Literal(T value);

    /// @brief Retrieve the captured value.
    T get() const;

    bool accepts(T value) const override { return true; }
    bool rejects(T value) const override { return false; }

    /// @brief Return the captured value. The input and parameters are ignored.
    T apply(T, P) override;

    /// @brief Return the captured value. The input and parameters are ignored.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    T resolve(InputT, P) override;
};

} // namespace solidfi
