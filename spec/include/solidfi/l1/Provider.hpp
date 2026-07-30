// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Provider.hpp
/// @ingroup solidfi_l1_extras

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @proposed
/// @brief A named concrete base for Converter.
///
/// Provider adds no behavior -- it is a named abstract base that subclasses implement.
/// Its original role as a participation contract for Solver and Router is under review:
/// with explicit linked-list wiring in Multipath and Path, a separate marker base class
/// may not be necessary. Status: proposed pending implementation experience.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Provider : public Converter<T, U, P> {
public:
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) noexcept override = 0;
};

} // namespace solidfi
