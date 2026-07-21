// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Solver.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Runtime.hpp"
#include "solidfi/l1/Path.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
/// @brief Typed path finder. IS-A Converter<Runtime, Path<T,U,P>, P>.
///
/// Solver is the engine of Parameterized Traversal (level 2 of 3). It is one of three
/// complementary concepts in the dynamic traversal group:
///
/// - **Solver<T,U,P>** — typed; finds a T->U route through a Runtime. IS-A Converter.
/// - **Pathfinder** — untyped; one instance, any T->U query. For runtime-unknown types.
/// - **Path<T,U,P>** — is the route. IS-A Converter<T,U,P>; executes when traversed.
/// - **Router<T,U,P>** — find-and-execute as a single Converter<T,U,P>.
///
/// Levels of traversal:
///
/// Level 1 - static reachability - asks whether any path T->U exists in the Runtime at all.
/// That is a structural property of the installed converters, independent of runtime data.
///
/// Level 2 - this - asks: given a specific value of T and parameters P, which path leads
/// to U? P is a routing signal: the same Runtime may offer multiple T->U paths, and P
/// selects among them. A converter's accepts/rejects/handles methods participate by
/// declining specific values or parameters, causing Solver to try the next edge.
///
/// Level 3 - Proxy (L2) - extends level 2 with proxy objects attached to converters from
/// outside, intercepting traversal without modifying the Runtime.
///
/// Because Solver IS-A Converter<Runtime, Path<T,U,P>, P>, it composes: multiple Solver
/// strategies can be installed in a Chain, each trying a different traversal algorithm,
/// first to return a valid Path wins.
///
/// Use Pathfinder when T and U are only known at runtime.
///
/// **Invariants:**
/// - Solver is responsible for cycle avoidance during traversal.
/// - Solver MUST NOT modify the Runtime passed to fetch().
/// - P flows into individual converters along the path, consistent with Converter semantics.
///   The Runtime itself is not inspected with P.
///
/// @tparam T source type; the start of the path being sought.
/// @tparam U destination type; the end of the path being sought.
/// @tparam P parameters type; flows into each Converter along the found path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Solver : public Converter<Runtime, Path<T, U, P>, P> {
public:
    /// @brief Find a path T->U through the given Runtime. Returns it without executing.
    ///
    /// The returned Path IS-A Converter<T,U,P> and can be traversed immediately or
    /// retained for reuse. Returns a failed Path if no route exists.
    ///
    /// To find-and-execute in one step, use Router<T,U,P>.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    Path<T, U, P> fetch(Runtime runtime, P params) override;
};

} // namespace solidfi
