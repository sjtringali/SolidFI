// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Selector.hpp
/// @ingroup solidfi_l1_extras

#include <vector>
#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @proposed{8/3/26}
/// @brief Extracts zero or more sub-values of type U from a containing value of type T.
///
/// Selector is a Converter<T, std::vector<U>, P> intended for use with Transform.
/// U is expected to be a sub-value of T -- a field, node, or element that lives
/// structurally inside T, not a derived or projected type.
///
/// In languages with reference semantics, the returned U values are typically
/// references into T, not copies. A Transform<U, P> applied to them updates T
/// in place without any reconstruction step. In a tree of objects, the selected
/// nodes are already inside the tree; in a struct, the selected fields are
/// already at their addresses.
///
/// Selector returns std::vector<U> (zero-or-more) to express that selection
/// may match any number of sub-values, including none.
///
/// @note IS-A: Converter<T, std::vector<U>, P>
/// @note Traversal maps naturally onto a Selector paired with a Transform<U, P>.
///
/// @tparam T the containing type; owns or holds one or more U values.
/// @tparam U the sub-value type; expected to be a structural part of T, not a projection.
/// @tparam P parameters type; defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Selector : public Converter<T, std::vector<U>, P> {
public:
    std::vector<U> resolve(T value, P params) noexcept override = 0;
};

} // namespace solidfi
