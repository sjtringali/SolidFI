// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Converter.hpp
/// @ingroup solidfi_l1_structural


#include "solidfi/l1/Parameters.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural

/// @accepted{5/24/26}
/// @brief Takes a value of type T and produces a value of type U.
///
/// T and U are typically different types — this is a genuine conversion, not a
/// transformation. Because T and U are distinct, failure is possible: there is no
/// identity fallback. Converter does not mandate a single failure representation,
/// resolve() returns a plain value of U, and what counts as failure is defined by
/// whoever composes converters (e.g. Chain's own `failed` value), not by Converter
/// itself.
///
/// P is an optional user-defined parameter type for routing and dispatch. The primitives 
/// does not inspect P, it only passes it through to resolve(). Defaults to Parameters.
///
/// **Filtering rule** (shared with Chain):
/// @code
///   if (accepts(value) && !rejects(value) && handles(params)) → attempt resolve()
/// @endcode
/// All three must pass; evaluation order is unspecified and implementation-defined.
/// A converter with none overridden is always attempted. Each method has one concern:
/// accepts() and rejects() MUST NOT depend on P; handles() MUST NOT depend on T.
///
/// **Invariants:**
/// - accepts(), rejects(), and handles() MUST be stateless and synchronous.
/// - accepts() and rejects() MUST NOT depend on P.
/// - handles() MUST NOT depend on T.
/// - resolve() MAY fail; the composing context (e.g. Chain) defines which value of
///   U represents failure.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
///   Passed by value — use `P = MyParams*` or `P = Shared<MyParams>` (L0) if sharing is needed.
template<typename T, typename U, typename P = Parameters>
class Converter {
public:
    /// @brief Returns true if this converter claims the input. Default: true.
    virtual bool accepts(T value) const noexcept { return true; }

    /// @brief Returns true if this converter explicitly refuses the input. Default: false.
    ///
    /// A converter that rejects is never attempted via resolve().
    virtual bool rejects(T value) const noexcept { return false; }

    /// @brief Returns true if this converter can handle these parameters. Default: true.
    ///
    /// A converter that does not handle the parameters is
    /// never attempted via resolve(). MUST NOT depend on T — that belongs in accepts().
    virtual bool handles(P params) const noexcept { return true; }

    /// @brief Perform the conversion. On failure, returns whichever value of U the
    /// composing context has defined to mean failure.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    /// @note Failure is state, not control flow — it is the returned value described above.
    virtual U resolve(T value, P params) = 0;

    // For use by Deferred
    using From = T;
    using To = U;
    using Parameters = P;
};

} // namespace solidfi
