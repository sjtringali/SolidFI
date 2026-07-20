#pragma once

/// @file Router.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Solver.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
/// @brief Find-and-execute as a single Converter<T,U,P>. Composes Solver with Path traversal.
///
/// Router is the third of three complementary concepts:
///
/// - **Solver** — finds the route. Returns a Path<T,U,P>; never executes it.
/// - **Path** — is the route. IS-A Converter<T,U,P>; executes when traversed.
/// - **Router** — does both. Holds a Solver; IS-A Converter<T,U,P> for the caller.
///
/// Use Solver directly when you want the Path before executing it (inspection, caching,
/// testing the route independent of execution). Use Router when the caller just needs
/// a Converter<T,U,P> and the find-then-execute detail is not their concern.
///
/// Because Router IS-A Converter<T,U,P>, it composes anywhere a Converter does: in a
/// Chain, in a Path step, or as a direct dependency. The dynamic graph search is hidden
/// behind the same interface as a single hand-written converter.
///
/// **Invariants:**
/// - Router MUST NOT modify the Solver or Graph it holds.
/// - Returns Sentinel<U>::value() if Solver finds no path or traversal fails.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Router : public Converter<T, U, P> {
public:
    /// @brief Bind this Router to a Solver.
    explicit Router(Solver<T, U, P> solver);

    bool accepts(T value) const override;
    bool rejects(T value) const override;

    /// @brief Solve for a path T->U and traverse it. Returns Sentinel<U>::value() on failure.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U fetch(T value, P params) override;
};

} // namespace solidfi
