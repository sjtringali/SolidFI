// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Roundtrip.hpp
/// @ingroup solidfi_l2

#include "solidfi/l1/Inverter.hpp"

namespace solidfi {

/// @ingroup solidfi_l2
/// @proposed
/// @brief Guarantees bidirectional communication between two endpoints.
///
/// Roundtrip is an Inverter<T,U> at the protocol level: forward() sends and awaits a
/// reply, reverse() replies in turn. Distinct from Codec — Codec pairs encode/decode of a
/// representation; Roundtrip pairs the two halves of a communication exchange.
///
/// @tparam T request type.
/// @tparam U response type.
///
/// @todo Specification pending. Parameters shape (Handshake?) and failure semantics TBD.
template<typename T, typename U>
class Roundtrip : public Inverter<T, U> {};

} // namespace solidfi
