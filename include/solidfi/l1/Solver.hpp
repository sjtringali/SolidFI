#pragma once

/// @file Solver.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Graph.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @brief Graph-bound path finder. Untyped: T and U are deduced at each call site.
///
/// Solver is the engine of Parameterized Traversal (level 2 of 3).
///
/// Level 1 - static reachability - asks whether any path T->U exists in the Graph at all.
/// That is a structural property of the installed converters, independent of runtime data.
///
/// Level 2 - this - asks: given a specific value of T and parameters P, which path leads
/// to U, and what does executing it produce? P is a routing signal: the same Graph may
/// offer multiple T->U paths, and P selects among them. A converter's accepts/rejects
/// methods participate by declining specific values, causing Solver to try the next edge.
///
/// Level 3 - Proxy (L2) - extends level 2 with proxy objects attached to converters from
/// outside, intercepting traversal without modifying the Graph.
///
/// Solver is untyped: T and U are resolved at each call site via template deduction.
/// One Solver instance can answer any T->U query against its bound Graph.
/// To expose a specific T->U path as a Converter<T,U,P>, use Router<T,U,P>.
///
/// The Graph is bound at construction time. Solver MUST NOT modify the Graph it holds.
///
/// **Invariants:**
/// - Solver is responsible for cycle avoidance during traversal.
/// - P flows into individual converters along the path, consistent with Converter semantics.
///   The Graph itself is not inspected with P.
///
/// @note Relationship to Traversal: Solver's public interface takes T directly (via solve()).
///   Traversal's interface is Converter<Graph,U,P> - it accepts Graph as input.
///   These are related concepts with different shapes; Solver holds the Graph internally.
class Solver {
public:
    /// @brief Bind this Solver to a specific Graph.
    explicit Solver(Graph graph);

    /// @brief Find a path T->U through the bound Graph and execute it.
    ///
    /// T and U are deduced at the call site. One Solver handles any T->U query.
    /// Returns Sentinel<U>::value() if no path exists or execution of the found path fails.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    template<typename T, typename U, typename P = Parameters>
    U solve(T value, P params);
};

} // namespace solidfi
