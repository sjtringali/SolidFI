#pragma once

/// @file Router.hpp
/// @ingroup solidfi_l1_runtime

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Graph.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_runtime
/// @brief A Converter<T,U,P> backed by a Graph. Pins T and U; enables the Composite rule.
///
/// Router is the typed projection of path-finding onto a specific T->U pair. Where Solver
/// is untyped and handles any query, Router fixes T and U at construction and IS-A
/// Converter<T,U,P> - to any caller holding a Converter<T,U,P> reference, a Router is
/// indistinguishable from a single converter.
///
/// Use Solver directly when T and U vary by call site (the common case).
/// Use Router when you need to hand off a specific T->U path as a Converter - for
/// composition via Chain, for testing substitution, or for passing to code that
/// holds only a Converter reference.
///
/// **Invariants:**
/// - Router MUST NOT modify the Graph it holds.
/// - Returns Failure<U>::value() if no path exists or execution fails.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Router : public Converter<T, U, P> {
public:
    /// @brief Bind this Router to a Graph.
    explicit Router(Graph graph);

    bool accepts(T value) const override;
    bool rejects(T value) const override;

    U fetch(T value, P params) override;
};

} // namespace solidfi
