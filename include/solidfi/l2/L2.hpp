#pragma once

/// @file L2.hpp
/// @brief SolidFI Level 2 — higher-level patterns. Named stubs only; not yet specified.
///
/// L2 concepts are built on L1 primitives. Each is a stub marking a named concept
/// whose full definition will be elaborated in a future revision.
///
/// @note P conventions: at L2, P can carry both inbound and outbound data — e.g.
///   `RequestResponse { in: Req, out: Res }` or `InOut<I,O>`. Converters along the path
///   read from `in` and write to `out` as traversal proceeds. This means round-trip and
///   request/response patterns require no new framework primitives — they are shaped P
///   values. The framework remains unchanged; the contract lives in P.

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
/// @brief Dynamic interception on a Converter — the detour on the road.
///
/// A Proxy intercepts a Converter's traversal without modifying the Graph. The graph
/// structure is unchanged; the Solver follows the route it found; but when it reaches
/// a proxied converter, the proxy intercepts before fetch() is called. The Solver does
/// not need to know. This is level 3 of Parameterized Traversal.
///
/// Two attachment modes:
/// - **Creation-bound**: the converter declares its own proxy at construction, defined
///   elsewhere. The converter knows it may be proxied; the proxy is part of its identity.
/// - **Injected**: a proxy is attached from outside after the fact — full hoisting. The
///   converter has no knowledge of it at creation time. Any holder of a converter reference
///   can install a proxy.
///
/// Both modes are valid. Full hoisting (injected) is the more powerful form.
///
/// Inspired by Qt's focusProxy (creation-bound) and installEventFilter (injected).
/// The proxy is data on the converter, not a subclass or wrapper. Dormant by default.
///
/// The abstract indirection concept lives at L0 as `Goto<T>`. Proxy is its
/// network-transparency specialization: a live service directory, remote redirection,
/// marshaling across process or network boundaries.
///
/// @todo Specification pending network transparency work, which will determine the final
///   structure. Both attachment modes are expected to survive; the API shape is TBD.
/// @note L0 root: Delegate<T> — pure object indirection, no network assumptions.
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
/// @brief A Graph populated with the edges valid for a specific deployment context.
///
/// Different deployment contexts (browser vs. Node, client vs. server) require different
/// edge sets — some converters cannot load or execute in a given environment. A Runtime
/// is the Graph for one such context: same converter vocabulary, only the edges that work here.
///
/// Assembly is direct: install the edges your environment supports. That assembly
/// generalizes into a `Transform<Graph>` (unconditional) or `Converter<Graph,Graph,P>`
/// (environment-driven) when you want to name, compose, or parameterize it.
///
/// @todo Specification pending.
class Runtime : public Graph {};

} // namespace solidfi
