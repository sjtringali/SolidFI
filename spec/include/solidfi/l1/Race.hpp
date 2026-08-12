// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Race.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @reserved{7/26/26}
/// @brief Runs all installed converters concurrently; the first to succeed wins.
///
/// Race is the concurrent sibling of Chain. Where Chain attempts
/// converters in priority order and short-circuits on the first success, Race attempts
/// all of them at once and short-circuits on whichever resolves first.
///
/// @todo Specification pending: install/remove/replace shape, and how Race, Chain, and
///   a future Parallel (concurrent, waits for all) share a common composition API.
///
/// @implements Converter<T, U, P>
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.

template<typename T, typename U, typename P = Parameters>
class Race : public Converter<T, U, P> {
public:
    U resolve(T value, P params) noexcept override = 0;
};

} // namespace solidfi
