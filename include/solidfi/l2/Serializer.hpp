// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Serializer.hpp
/// @ingroup solidfi_l2

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l2
/// @proposed
/// @brief Converts an in-memory representation to a serialized form. One-way.
///
/// Serializer<T,U> is a named boundary marker for Converter<T,U> — the "in-memory to wire
/// form" crossing, typically a step in a Graph path when crossing a process or network
/// boundary. Deliberately just a name for the slot: no behavior beyond Converter<T,U>,
/// because there is none to add.
///
/// @tparam T in-memory representation.
/// @tparam U serialized form (bytes, string, stream).
template<typename T, typename U>
using Serializer = Converter<T, U>;

} // namespace solidfi
