// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Traversal.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Runtime.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @reserved
/// @brief An algorithm that accepts a Runtime and produces a result of type U.
///
/// Traversal is Converter<Runtime, U, P> — the Runtime is the input type, and the
/// traversal algorithm produces some typed result. What U is depends on the specific
/// traversal: it may be a bool (reachability check), a sequence (path), an executed
/// result, or any other type.
///
/// Because Traversal IS a Converter, traversals compose transparently wherever a
/// Converter is expected. Multiple traversal strategies can be installed in a Chain.
///
/// @tparam U destination type; the result type produced by this traversal.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
///
/// @see Solver — a specific Traversal strategy that finds and executes a path T→U.
template<typename U, typename P = Parameters>
class Traversal : public Converter<Runtime, U, P> {
public:
    bool accepts(Runtime runtime) const override { return true; }
    bool rejects(Runtime runtime) const override { return false; }

    /// @brief Execute the traversal over the given runtime with the given parameters.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(Runtime runtime, P params) override = 0;
};

} // namespace solidfi
