// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Multipath.hpp
/// @ingroup solidfi_l1_compositions
/// @todo Construction API is provisional. Chained builder may be replaced with
///       non-chained install() calls, similar to Chain. Shape will be clarified
///       once real implementation settles.

#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Pipeline.hpp"
#include "solidfi/l1/Transform.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
/// @brief A wired, multi-stage conversion path that may include branching. IS-A Converter<T,U,P>.
///
/// Where Solver finds a path through a Domain at runtime, Multipath declares one at
/// construction. The path exists because you made it. Intermediate types thread through
/// at compile time via recursive template composition -- no runtime loop, no discovery.
/// The result is always a Converter<T,U,P>: one traverse(), one answer.
///
/// Branching is supported at any stage via toEither(), which embeds a Chain at that
/// position. Path is the non-branching specialization: Path IS-A Multipath.
///
/// Steps are added via the builder interface:
/// - to()         -- advances the type (T->V) by adding a single Converter.
/// - toEither()   -- advances the type (T->V) by adding a Chain; first to succeed wins.
/// - through()    -- holds the type (T->T) by adding a single Transform.
/// - throughAll() -- holds the type (T->T) by composing multiple Transforms into a Pipeline.
///
/// Prefer Multipath when any stage may need a Chain. Prefer Path when the route is fully
/// deterministic. Prefer Solver when the route is discovered or varies at runtime.
///
/// Internally, Multipath holds a linked list of steps. The head is typed T (the entry
/// point) and the tail is typed U (the exit point); intermediate types are captured by
/// the recursive node structure and not visible at the Multipath level. Steps may be
/// wrapped in a Proxy on insertion, enabling transparent interception at traversal time.
/// Access to the endpoints is via the protected start() and finish() methods.
///
/// **Invariants:**
/// - The path is defined at construction. Steps are not added after build.
/// - Failure at any stage propagates as whatever failure value that stage's
///   Converter or Chain defines for U -- the path does not retry.
/// - Multipath MUST NOT modify any Converter or Transform it holds.
/// - The internal structure is a linked list (cons-list of steps). No runtime iteration.
///
/// @tparam T source (start) type; input type of the first step.
/// @tparam U destination (end) type; output type of the last step.
/// @tparam P parameters type; flows into every Converter along the path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Multipath : public Converter<T, U, P> {
public:
    /// @brief Add a single Converter that advances the current end type to V.
    ///
    /// Returns Multipath<T,V,P> so further calls can be chained.
    template<typename V>
    Multipath<T, V, P> to(Converter<U, V, P> converter);

    /// @brief Add a Chain of Converters that advance the current end type to V.
    ///
    /// Multiple converters are tried in order; the first to succeed is used.
    /// Declares a Chain<U,V,P> at this position.
    /// Returns Multipath<T,V,P> so further calls can be chained.
    template<typename V, typename... Cs>
    Multipath<T, V, P> toEither(Cs... converters);

    /// @brief Add a Transform that holds the current end type.
    ///
    /// Returns Multipath<T,U,P> so further calls can be chained.
    Multipath<T, U, P> through(Transform<U, P> transform);

    /// @brief Add N > 1 Transforms that hold the current end type, composed into a Pipeline.
    ///
    /// Requires at least two transforms -- use through() for a single transform.
    /// Returns Multipath<T,U,P> so further calls can be chained.
    template<typename... Ts>
    Multipath<T, U, P> throughAll(Ts... transforms);

    /// @brief Execute the path. Friendly alias for resolve().
    ///
    /// Prefer traverse() when calling a known Multipath directly.
    U traverse(T value, P params) noexcept;

    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) noexcept override;

protected:
    /// @brief Returns a pointer to the head of the internal step list (input side, typed T).
    ///
    /// The returned pointer may refer to a Proxy wrapping the actual first Converter --
    /// the distinction is transparent to callers. Named start()/finish() to avoid
    /// collision with STL container front()/back() conventions.
    virtual T* start() const;

    /// @brief Returns a pointer to the tail of the internal step list (output side, typed U).
    virtual U* finish() const;
};

/// @brief Begin building a Multipath starting from type T.
///
/// Entry point for the builder:
/// @code
///   auto p = multipath<Filename>().to(reader).toEither(unzipper, fallback).to(parser);
///   HTML html = p.traverse("document.odt", {});
/// @endcode
template<typename T, typename P = Parameters>
Multipath<T, T, P> multipath();

} // namespace solidfi
