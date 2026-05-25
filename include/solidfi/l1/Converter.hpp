#pragma once

/// @file Converter.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Parameters.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Takes a value of type T and produces a value of type U.
///
/// T and U are typically different types — this is a genuine conversion, not a
/// transformation. Because T and U are distinct, failure is possible: there is no
/// identity fallback. Failure is represented by an absent `Optional<U>`.
///
/// Failure is represented as a sentinel value of U declared via Failure<U> — a
/// non-intrusive specialization (see Failure.hpp). Converter itself does not require
/// Failure<U>; that contract belongs to Chain, which uses it to test fetch() results
/// during traversal. Failure is state, not control flow — fetch() never throws.
///
/// P is an optional user-defined parameter type for routing and dispatch. The framework
/// never inspects P — it only passes it through to fetch(). Defaults to Parameters.
///
/// **Filtering rule** (shared with Chain):
/// @code
///   if (!rejects(value) && accepts(value)) → attempt fetch()
/// @endcode
/// accepts() and rejects() are optimizations — a converter with neither implemented is
/// always attempted via fetch(). They MUST be stateless, synchronous, and side-effect-free.
/// They MUST NOT depend on P; that logic belongs in fetch().
///
/// **Invariants:**
/// - accepts() and rejects() MUST be stateless and synchronous.
/// - accepts() and rejects() MUST NOT depend on P.
/// - fetch() MAY fail; returns Failure<U>::value() on failure. MUST NOT throw.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
///   Passed by value — use `P = MyParams*` or `P = Shared<MyParams>` (L0) if sharing is needed.
template<typename T, typename U, typename P = Parameters>
class Converter {
public:
    /// @brief Returns true if this converter claims the input. Default: true.
    virtual bool accepts(T value) const { return true; }

    /// @brief Returns true if this converter explicitly refuses the input. Default: false.
    ///
    /// Evaluated before accepts(). A converter that rejects is never attempted via fetch().
    virtual bool rejects(T value) const { return false; }

    /// @brief Perform the conversion. Returns Failure<U>::value() on failure.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    /// @note Never throws. Failure is a returned value, not a control flow path.
    virtual U fetch(T value, P params) = 0;

};

} // namespace solidfi
