// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Codec.hpp
/// @ingroup solidfi_l2

#include "solidfi/l1/Inverter.hpp"
#include "solidfi/l2/Serializer.hpp"

namespace solidfi {

/// @ingroup solidfi_l2
/// @proposed
/// @brief A Serializer that also decodes: encode and decode as a matched pair.
///
/// Codec is an Inverter<T,U> — forward() encodes, reverse() decodes. Because Inverter<T,U>
/// IS-A Converter<T,U>, a Codec satisfies Serializer<T,U> wherever only the one-way
/// direction is needed, with no extra wiring:
///
/// @code
///   // Consumer only needs to serialize. Written against the one-way interface.
///   void writeToDisk(Serializer<Order, Bytes>& serializer, Order order, Parameters params) {
///       Bytes out = serializer.resolve(order, params);
///       // ...
///   }
///
///   class JsonCodec : public Codec<Order, Bytes> {
///   public:
///       Bytes forward(Order order, Parameters params) override { /* encode */ }
///       Order reverse(Bytes bytes, Parameters params) override { /* decode */ }
///   };
///
///   JsonCodec codec;
///   writeToDisk(codec, order, params);  // codec IS-A Serializer<Order,Bytes>, via Inverter
/// @endcode
///
/// @tparam T in-memory representation.
/// @tparam U serialized form.
template<typename T, typename U>
class Codec : public Inverter<T, U> {};

} // namespace solidfi
