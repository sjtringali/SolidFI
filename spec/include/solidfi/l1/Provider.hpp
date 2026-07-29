// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Provider.hpp
/// @ingroup solidfi_l1_extras

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @accepted
/// @brief A named base class for Converter. Extending Provider is the participation contract.
///
/// Provider adds no behavior. It exists so that components like the Solver and Router can
/// distinguish registered converters from anonymous ones. If you want to participate in
/// graph-based routing or solver discovery, extend Provider. If you only need to satisfy
/// a Converter<T,U,P> slot directly, a plain Converter implementation is sufficient.
///
/// What Provider means in practice is decided by the components that consume it. Provider
/// itself is intentionally neutral.
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
