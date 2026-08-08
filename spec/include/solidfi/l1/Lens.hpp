// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Lens.hpp
/// @ingroup solidfi_l1_extras

#include <vector>
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Transform.hpp"
#include "solidfi/l1/Selector.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @proposed{8/3/26}
/// @brief Concrete composition of a Selector with an optional Transform, usable as either.
///
/// Lens<T, U, P> is the concrete counterpart to Selector, in the same way that
/// Chain is the concrete counterpart to Converter and Pipeline is the concrete
/// counterpart to Transform.
///
/// | Abstract      | Concrete composition |
/// |---------------|----------------------|
/// | Converter<T,U>| Chain<T,U>           |
/// | Transform<T>  | Pipeline<T>          |
/// | Selector<T,U> | Lens<T,U>            |
///
/// Without an inner Transform, Lens IS-A Converter<T, std::vector<U>, P>:
/// it focuses on the sub-values selected by the Selector and returns them.
///
/// With an inner Transform<U, P>, Lens also IS-A Transform<T, P>:
/// it applies the transform to each selected sub-value and returns the
/// (modified) container. In languages with reference semantics this
/// reconstruction is implicit -- the sub-values are already inside T.
///
/// Lens is concrete: construct it with the pieces, use it anywhere a
/// Converter<T, std::vector<U>, P> or Transform<T, P> is expected.
///
/// @note IS-A: Converter<T, std::vector<U>, P>
/// @note IS-A: Transform<T, P> when constructed with an inner Transform<U, P>.
///
/// @tparam T the containing type.
/// @tparam U the sub-value type; a structural part of T, not a projection.
/// @tparam P parameters type; defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Lens
    : public Converter<T, std::vector<U>, P>
    , public Transform<T, P> {
public:
    /// @brief Construct a read-only Lens. IS-A Converter<T, std::vector<U>, P> only.
    explicit Lens(Selector<T, U, P> selector);

    /// @brief Construct a read-write Lens. IS-A both Converter and Transform.
    Lens(Selector<T, U, P> selector, Transform<U, P> transform);

    /// @brief Extract sub-values from the container. Converter half.
    std::vector<U> resolve(T value, P params) noexcept override;

    /// @brief Apply the inner transform to each selected sub-value. Transform half.
    /// @note Requires construction with an inner Transform; throws otherwise.
    T apply(T value, P params) noexcept override;
};

} // namespace solidfi
