#pragma once

/// @file L2.hpp
/// @brief SolidFI Level 2 — higher-level patterns. Named stubs only; not yet specified.
///
/// L2 concepts are built on L1 primitives. Each is a stub marking a named concept
/// whose full definition will be elaborated in a future revision.

namespace solidfi {

/// @defgroup solidfi_l2 L2
/// @brief Level 2 — higher-level patterns built on L1 primitives.

/// @ingroup solidfi_l2
/// @brief Defines the encoding and framing contract for a communication exchange.
///
/// Protocol governs how data is represented on the wire — framing, versioning,
/// encoding format. Composed with Transport to form a complete communication layer.
///
/// @todo Specification pending.
class Protocol {};

/// @ingroup solidfi_l2
/// @brief Handles physical or logical transmission of data between endpoints.
///
/// Transport abstracts the channel — TCP, HTTP, in-process queue, shared memory.
/// Composed with Protocol to form a complete communication layer.
///
/// @todo Specification pending.
class Transport {};

/// @ingroup solidfi_l2
/// @brief Converts between in-memory representations and serialized forms.
///
/// Serialization is a Converter concern: T (in-memory) ↔ bytes/string/stream.
/// Typically used as a step in a Graph path when crossing a process or network boundary.
///
/// @todo Specification pending.
class Serialization {};

/// @ingroup solidfi_l2
/// @brief Optional interception on a Converter — opt-in, default-off.
///
/// Proxy allows a Converter to be intercepted before or instead of its own fetch(),
/// enabling network marshaling, caching, retry, logging, or rerouting without
/// modifying the converter or the graph structure.
///
/// Inspired by Qt's intrusive proxy and event-filter patterns: each Converter is aware
/// it might be proxied, but the proxy is data — a field you set, not a method you override.
/// Dormant by default; awakened by installation.
///
/// @todo Specification pending. Design open: embed on Converter (Qt intrusive style) vs.
///   delegate object.
class Proxy {};

/// @ingroup solidfi_l2
/// @brief Guarantees bidirectional communication between two endpoints.
///
/// Roundtrip builds on L1 Inverter<T,U>, which guarantees bidirectional conversion
/// (T→U and U→T at the type level), and elevates that guarantee to the protocol level:
/// a full round trip — serialize, transmit, receive, deserialize — with the inverse path
/// guaranteed to exist.
///
/// @todo Specification pending.
class Roundtrip {};

/// @ingroup solidfi_l2
/// @brief Graph extended with dynamic plugin-loading machinery.
///
/// Runtime is an L1 Graph extended with the capability to dynamically install and remove
/// converters at runtime, enabling plugin architectures. A plugin is just a Converter —
/// it has type signatures and predicates, and the Solver routes through it like any other edge.
///
/// @todo Specification pending.
class Runtime {};

} // namespace solidfi
