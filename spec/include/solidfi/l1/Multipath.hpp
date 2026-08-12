// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Multipath.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Path.hpp"
#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/Pipeline.hpp"
#include "solidfi/l1/Transform.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed{8/4/26}
/// @brief Construction helper that builds a Path with inline Chain and Pipeline sugar.
///        IS-A Path<T,U,P> and therefore IS-A Converter<T,U,P>.
///
/// Path accepts any Converter at each step, including Chain -- but wiring a Chain
/// inline requires constructing it separately. Multipath removes that ceremony with
/// toEither() and throughAll(), which build the Chain or Pipeline in place.
///
/// Every method returns Multipath<T,V,P> so calls can be chained. The result is a
/// fully usable Path<T,U,P>; Multipath adds nothing at runtime.
///
/// Use Multipath when building complex paths with multiple branching or transform
/// steps. Use Path directly when steps are already constructed or branching is
/// not needed.
///
/// @extends Path<T, U, P>
///
/// @tparam T source (start) type.
/// @tparam U destination (end) type.
/// @tparam P parameters type. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Multipath : public Path<T, U, P> {
public:
    /// @brief Append a single Converter, advancing the end type to I.
    template<typename I>
    Multipath<T, I, P> to(Converter<U, I, P> converter);

    /// @brief Append an inline Chain of Converters, advancing the end type to I.
    ///
    /// Equivalent to constructing a Chain<U,I,P>, installing each converter in order,
    /// and appending it. The first to succeed at runtime wins.
    template<typename I, typename... Cs>
    Multipath<T, I, P> toEither(Cs&&... converters);

    /// @brief Append a single Transform, holding the current end type.
    Multipath<T, U, P> through(Transform<U, P> transform);

    /// @brief Append an inline Pipeline of Transforms, holding the current end type.
    ///
    /// Equivalent to constructing a Pipeline<U,P>, installing each transform in order,
    /// and appending it. Requires at least two transforms -- use through() for one.
    template<typename... Ts>
    Multipath<T, U, P> throughAll(Ts&&... transforms);
};

} // namespace solidfi
