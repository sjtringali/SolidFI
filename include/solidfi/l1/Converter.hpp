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
/// identity fallback. Sentinel is represented as a sentinel value of U declared via Sentinel<U> — a
/// non-intrusive specialization (see Sentinel.hpp). Converter itself does not require
/// Sentinel<U>; that contract belongs to Chain, which uses it to test fetch() results
/// during traversal. Sentinel is state, not control flow — fetch() never throws.
///
/// P is an optional user-defined parameter type for routing and dispatch. The framework
/// never inspects P — it only passes it through to fetch(). Defaults to Parameters.
///
/// **Filtering rule** (shared with Chain):
/// @code
///   if (accepts(value) && !rejects(value) && handles(params)) → attempt fetch()
/// @endcode
/// All three must pass; evaluation order is unspecified and implementation-defined.
/// A converter with none overridden is always attempted. Each method has one concern:
/// accepts() and rejects() MUST NOT depend on P; handles() MUST NOT depend on T.
///
/// **Invariants:**
/// - accepts(), rejects(), and handles() MUST be stateless and synchronous.
/// - accepts() and rejects() MUST NOT depend on P.
/// - handles() MUST NOT depend on T.
/// - fetch() MAY fail; returns Failed<T> on failure. MUST NOT throw.
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

    /// @brief Returns true if this converter can handle these parameters. Default: true.
    ///
    /// Evaluated after accepts(). A converter that does not handle the parameters is
    /// never attempted via fetch(). MUST NOT depend on T — that belongs in accepts().
    virtual bool handles(P params) const { return true; }

    /// @brief Perform the conversion. Returns Failed<T> on failure.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    /// @note Never throws. Sentinel is a returned value, not a control flow path.
    virtual U fetch(T value, P params) = 0;

};

} // namespace solidfi
