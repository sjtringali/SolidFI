#pragma once

/// @file Blazer.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Pipeline.hpp"
#include "solidfi/l1/Transform.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @brief An explicitly-wired, multi-stage conversion path. Is itself a Converter<T,U,P>.
///
/// Where Solver finds a path through a Graph at runtime, Blazer declares one at construction.
/// The path exists because you made it. Intermediate types thread through at compile time
/// via recursive template composition — no runtime loop, no discovery. The result is always
/// a Converter<T,U,P>: one fetch(), one answer.
///
/// Steps are added via the fluent builder interface. Converters advance the type (T->V);
/// Transforms hold it (T->T). Consecutive converters of the same type collapse into a
/// Chain<T,U,P>; consecutive transforms collapse into a Pipeline<T>. Collapsing is internal
/// — the Blazer's external shape is always Converter<Start,End,P>.
///
/// Blazer is the static complement to Solver: prefer Blazer when the path is known and
/// fixed; prefer Solver when the path is discovered or varies at runtime.
///
/// **Invariants:**
/// - The path is defined at construction. Steps are not added after build.
/// - Failure at any stage propagates as Sentinel<U>::value() — the path does not retry.
/// - Blazer MUST NOT modify any Converter or Transform it holds.
/// - The internal structure is recursive (cons-list of steps). No runtime iteration.
///
/// @tparam T source (start) type; input type of the first step.
/// @tparam U destination (end) type; output type of the last step.
/// @tparam P parameters type; flows into every Converter along the path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Blazer : public Converter<T, U, P> {
public:
    /// @brief Add one or more Converters that advance the current end type to V.
    ///
    /// A single argument adds a converter directly. Multiple arguments declare a Chain<U,V,P>
    /// at this position, the grouping is explicit at the call site.
    /// Returns Blazer<T,V,P> so further via() and through() calls can be chained.
    template<typename V, typename... Cs>
    Blazer<T, V, P> via(Cs... converters);

    /// @brief Add one or more Transforms that hold the current end type.
    ///
    /// A single argument adds a transform directly. Multiple arguments declare a Pipeline<U>
    /// at this position, the grouping is explicit at the call site.
    /// Returns Blazer<T,U,P> so further via() and through() calls can be chained.
    template<typename... Ts>
    Blazer<T, U, P> through(Ts... transforms);

    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U fetch(T value, P params) override;
};

/// @brief Begin building a Blazer starting from type T.
///
/// Entry point for the fluent builder:
/// @code
///   auto path = blaze<Filename>().via(reader).via(unzipper).via(parser).via(renderer);
///   HTML html = path.fetch("document.odt", {});
/// @endcode
///
/// In C++, the full intermediate type list may also be provided as a variadic pack
/// when the path is known entirely at the call site.
template<typename T, typename P = Parameters>
Blazer<T, T, P> blaze();

} // namespace solidfi
