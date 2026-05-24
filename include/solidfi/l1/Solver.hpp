#pragma once

/// @file Solver.hpp
/// @ingroup solidfi_l1_runtime

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Graph.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_runtime
/// @brief A Converter<T,U,P> backed by a Graph. Finds and executes a path from T to U.
///
/// To any caller holding a Converter<T,U,P> reference, a Solver is indistinguishable from
/// a single converter — the Composite rule.
///
/// Given a value of type T and parameters P, Solver uses the bound Graph to find a path
/// T → ... → U through the installed converters and executes it. The path discovery and
/// the execution are both internal to Solver.
///
/// The Graph is bound at construction time. Solver MUST NOT modify the Graph it holds.
///
/// **Invariants:**
/// - If no path exists, fetch() MUST return an absent `Optional<U>`.
/// - If a path exists but execution fails at any step, fetch() MUST return an absent `Optional<U>`.
/// - Solver is responsible for cycle avoidance during traversal.
/// - P flows into individual converters along the path, consistent with Converter semantics.
///   The Graph itself is not inspected with P.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
///
/// @note Relationship to Traversal: Solver's public interface is Converter<T,U,P> (input is T).
///   Traversal's interface is Converter<Graph,U,P> (input is Graph). These are related concepts
///   with different shapes; Solver holds the Graph internally rather than accepting it as input.
template<typename T, typename U, typename P = Parameters>
class Solver : public Converter<T, U, P> {
public:
    /// @brief Bind this Solver to a specific Graph.
    explicit Solver(Graph& graph);

    bool accepts(T value) const override;
    bool rejects(T value) const override;

    /// @brief Find a path T→U through the bound Graph and execute it with the given parameters.
    ///
    /// Returns absent value if no path exists or if execution of the found path fails.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    Optional<U> fetch(T value, P params) override;
};

} // namespace solidfi
