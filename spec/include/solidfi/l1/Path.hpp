// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Path.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Multipath.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @accepted
/// @brief A single, non-branching wired route from T to U. IS-A Multipath<T,U,P>.
///
/// Path is the strict form of Multipath: every step is deterministic -- no toEither(),
/// no Chain at any position. The route is fully declared at construction; the result is
/// a Converter<T,U,P>: one traverse(), one answer.
///
/// Because Path IS-A Multipath IS-A Converter<T,U,P>, it satisfies both type slots.
/// Solver<T,U,P> produces a Path<T,U,P>.
///
/// Builder interface -- mirrors Multipath without toEither() or varargs:
/// - to()      -- advances the type (T->V) by adding a single Converter.
/// - through() -- holds the type (T->T) by adding a single Transform.
///
/// Each step returns Path<T,...,P> rather than Multipath<T,...,P>.
///
/// **Invariants:**
/// - The path is defined at construction. Steps are not added after build.
/// - Failure at any stage propagates as whatever failure value that stage's Converter
///   defines for U -- the path does not retry.
/// - Path MUST NOT modify any Converter or Transform it holds.
///
/// @tparam T source (start) type; input type of the first step.
/// @tparam U destination (end) type; output type of the last step.
/// @tparam P parameters type; flows into every Converter along the path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Path : public Multipath<T, U, P> {
public:
    /// @brief Add a single Converter that advances the current end type to V.
    ///
    /// Returns Path<T,V,P> so further calls can be chained.
    template<typename V>
    Path<T, V, P> to(Converter<U, V, P> converter);

    /// @brief Add a Transform that holds the current end type.
    ///
    /// Returns Path<T,U,P> so further calls can be chained.
    Path<T, U, P> through(Transform<U, P> transform);

    /// @brief Execute the path. Friendly alias for resolve().
    ///
    /// Prefer traverse() when calling a known Path directly.
    U traverse(T value, P params) noexcept;

    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) noexcept override;

protected:
    T* start() const override;
    U* finish() const override;
};

/// @brief Begin building a Path starting from type T.
///
/// Entry point for the builder:
/// @code
///   auto p = path<Filename>().to(reader).to(unzipper).to(parser).to(renderer);
///   HTML html = p.traverse("document.odt", {});
/// @endcode
template<typename T, typename P = Parameters>
Path<T, T, P> path();

} // namespace solidfi
