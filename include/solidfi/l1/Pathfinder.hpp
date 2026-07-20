// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Pathfinder.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Graph.hpp"
#include "solidfi/l1/Parameters.hpp"
#include "solidfi/l1/Path.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
/// @brief Untyped path finder. Graph-bound; one instance, any T->U query.
///
/// Pathfinder is the dynamic complement to Solver<T,U,P>. Where Solver fixes T and U
/// at compile time, Pathfinder deduces them at each call site. Use Pathfinder when the
/// types are only known at runtime — for example, a dispatch table keyed on runtime type
/// information, or a system that builds queries from user input.
///
/// Without Pathfinder, handling runtime-unknown T->U requires a typed Solver per pair
/// plus a picker to choose among them — the combinatorial expansion the spec exists to
/// prevent. Pathfinder absorbs that case the same way Chain absorbs a hand-written
/// converter dispatcher.
///
/// Because T and U are not fixed, Pathfinder cannot IS-A Converter. For a composable
/// Converter<T,U,P> over a known pair, use Solver<T,U,P>. For find-and-execute, use
/// Router<T,U,P>.
///
/// The Graph is bound at construction time. Pathfinder MUST NOT modify the Graph it holds.
///
/// **Invariants:**
/// - Pathfinder is responsible for cycle avoidance during traversal.
/// - P flows into individual converters along the path, consistent with Converter semantics.
///   The Graph itself is not inspected with P.
class Pathfinder {
public:
    /// @brief Bind this Pathfinder to a specific Graph.
    explicit Pathfinder(Graph graph);

    /// @brief Find a path T->U through the bound Graph. Returns it without executing.
    ///
    /// T and U are deduced at the call site. One Pathfinder handles any T->U query.
    /// Returns a failed Path if no route exists.
    ///
    /// To find-and-execute in one step, use Router<T,U,P>.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    template<typename T, typename U, typename P = Parameters>
    Path<T, U, P> find(T value, P params);
};

} // namespace solidfi
