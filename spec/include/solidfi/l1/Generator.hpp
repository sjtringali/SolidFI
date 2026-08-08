// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Generator.hpp
/// @ingroup solidfi_l1_extras

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @reserved{7/18/26}
/// @brief Produces a value of type T from nothing.
///
/// Generator is conceptually Converter<void, T, P> — a conversion with no meaningful input.
/// Because void is not a valid C++ template type argument, the input type is modeled as
/// Void, a tag type carrying no data. Callers pass Void{} as the input value.
///
/// @tparam T destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename P = Parameters>
class Generator : public Converter<Void, T, P> {
public:
    bool accepts(Void) const noexcept override { return true; }
    bool rejects(Void) const noexcept override { return false; }

    /// @brief Produce a T. The Void argument carries no data and should be ignored.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    T resolve(Void input, P params) noexcept override = 0;
};

} // namespace solidfi
