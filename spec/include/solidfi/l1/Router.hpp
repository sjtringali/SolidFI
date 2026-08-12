// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Router.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Domain.hpp"
#include "solidfi/l1/Solver.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @history{Reserved 5/26/26}
/// @proposed{7/19/26}
/// @brief Find-and-execute as a single Converter<T,U,P>. Composes Solver with Path traversal.
///
/// Router is the third of three complementary concepts:
///
/// - **Solver** — finds the route. Returns a Path<T,U,P>; never executes it.
/// - **Path** — is the route. IS-A Multipath<T,U,P> IS-A Converter<T,U,P>; executes when traversed.
/// - **Router** — does both. Holds a Solver; IS-A Converter<T,U,P> for the caller.
///
/// Use Solver directly when you want the Path before executing it (inspection, caching,
/// testing the route independent of execution). Use Router when the caller just needs
/// a Converter<T,U,P> and the find-then-execute detail is not their concern.
///
/// Because Router IS-A Converter<T,U,P>, it composes anywhere a Converter does: in a
/// Chain, in a Multipath step, or as a direct dependency. The dynamic graph search is hidden
/// behind the same interface as a single hand-written converter.
///
/// @invariant Router MUST NOT modify the Solver or Domain it holds.
/// @invariant Router carries its own failed value (see the constructor), returned when
///   Solver finds no path at all, or when the resulting Path's traversal fails.
///
/// @implements Converter<T, U, P>
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Router : public Converter<T, U, P> {
public:
    /// @brief Bind this Router to a Domain. Creates a default Solver internally.
    explicit Router(Domain domain);

    /// @brief Bind this Router to a Domain, with an explicit failure value.
    /// @param failed Value of U returned when Solver finds no path or traversal fails.
    Router(Domain domain, U failed);

    /// @brief Bind this Router to an explicit Solver. Use when traversal strategy matters.
    explicit Router(Solver<T, U, P> solver);

    /// @brief Bind this Router to an explicit Solver, with an explicit failure value.
    /// @param failed Value of U returned when Solver finds no path or traversal fails.
    Router(Solver<T, U, P> solver, U failed);

    bool accepts(T value) const noexcept override;
    bool rejects(T value) const noexcept override;

    /// @brief Solve for a path T->U and traverse it. On failure (no path found, or
    /// traversal fails), returns Router's own failed value.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    /// @retval failed Router's own configured failed value, if Solver finds no path or traversal fails.
    U resolve(T value, P params) noexcept override;

private:
    U failed;
};

} // namespace solidfi
