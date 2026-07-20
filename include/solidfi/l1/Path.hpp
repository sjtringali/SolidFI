#pragma once

/// @file Path.hpp
/// @ingroup solidfi_l1_compositions
/// @todo Construction API is provisional. Chained builder may be replaced with
///       non-chained install() calls, similar to Chain. Path may converge toward
///       Chain's construction model once real implementation clarifies the shape.

#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Pipeline.hpp"
#include "solidfi/l1/Transform.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
/// @brief An explicitly-wired, multi-stage conversion path. Is itself a Converter<T,U,P>.
///
/// Where Solver finds a path through a Graph at runtime, Path declares one at construction.
/// The path exists because you made it. Intermediate types thread through at compile time
/// via recursive template composition - no runtime loop, no discovery. The result is always
/// a Converter<T,U,P>: one traverse(), one answer.
///
/// Steps are added via the builder interface:
/// - to() advances the type (T->V) by adding a single Converter.
/// - toEither() advances the type (T->V) by adding a Chain of Converters — first to succeed wins.
/// - through() holds the type (T->T) by adding a single Transform.
/// - throughAll() holds the type (T->T) by composing multiple Transforms into a Pipeline.
///
/// Path is the static complement to Solver: prefer Path when the route is known and
/// fixed; prefer Solver when the route is discovered or varies at runtime.
///
/// **Invariants:**
/// - The path is defined at construction. Steps are not added after build.
/// - Failure at any stage propagates as Sentinel<U>::value() - the path does not retry.
/// - Path MUST NOT modify any Converter or Transform it holds.
/// - The internal structure is recursive (cons-list of steps). No runtime iteration.
///
/// @tparam T source (start) type; input type of the first step.
/// @tparam U destination (end) type; output type of the last step.
/// @tparam P parameters type; flows into every Converter along the path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Path : public Converter<T, U, P> {
public:
    /// @brief Add a single Converter that advances the current end type to V.
    ///
    /// Returns Path<T,V,P> so further calls can be chained.
    template<typename V>
    Path<T, V, P> to(Converter<U, V, P> converter);

    /// @brief Add a Chain of Converters that advance the current end type to V.
    ///
    /// Multiple converters are tried in order; the first to succeed is used.
    /// Declares a Chain<U,V,P> at this position.
    /// Returns Path<T,V,P> so further calls can be chained.
    template<typename V, typename... Cs>
    Path<T, V, P> toEither(Cs... converters);

    /// @brief Add a Transform that holds the current end type.
    ///
    /// Returns Path<T,U,P> so further calls can be chained.
    /// C++ implementations may accept N transforms as a vararg overload of through().
    Path<T, U, P> through(Transform<U, P> transform);

    /// @brief Add N > 1 Transforms that hold the current end type, composed into a Pipeline.
    ///
    /// Requires at least two transforms — use through() for a single transform.
    /// Returns Path<T,U,P> so further calls can be chained.
    template<typename... Ts>
    Path<T, U, P> throughAll(Ts... transforms);

    /// @brief Execute the path. Friendly alias for fetch().
    ///
    /// Prefer traverse() when calling a known Path directly.
    U traverse(T value, P params);

    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U fetch(T value, P params) override;
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
