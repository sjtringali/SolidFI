// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file L0.hpp
/// @brief SolidFI Level 0 — substrate concepts. Named stubs only; not implemented here.
///
/// L0 defines the foundational primitives that inform L1. Each concept is declared as a
/// stub with a note describing its relationship to L1. L0 and L1 are independent — L1
/// does not include or depend on L0. Do not include this alongside L1 headers.

#include <string>
#include <vector>

/// @defgroup solidfi_l0 L0
/// @brief The substrate — named abstract concepts that underlie L1.
/// L0 defines the vocabulary of the spec without prescribing implementation. Each concept
/// is a stub: it claims an identity and notes its relationship to L1, but imposes no
/// structure on how it is realized. L0 is for implementers — a reference layer that names
/// what L1 concretizes. L0 and L1 are independent.

namespace solidfi {  // NOLINT: intentional overlap with L1; include L0 standalone only

/// @ingroup solidfi_l0
/// @brief A unit of executable work over a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Operation<T> — the core of Transform::apply and Converter::resolve.
template<typename T>
class Operation {
public:
    virtual void execute() = 0;
};

/// @ingroup solidfi_l0
/// @brief A single binary decision over a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Predicate<T> — splits into accepts() and rejects() in L1, each
///   declared inline on Transform and Converter with default bodies.
template<typename T>
class Predicate {
public:
    virtual bool decide(T value) = 0;
};

/// @ingroup solidfi_l0
/// @brief A pair of filtering decisions over T: acceptance and explicit rejection.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Filter<T> — becomes the inline accepts()/rejects() methods declared
///   directly on Transform<T> and Converter<T,U,P>.
template<typename T>
class Filter {
public:
    virtual bool accepts(T value) = 0;
    virtual bool rejects(T value) = 0;
};

/// @ingroup solidfi_l0
/// @brief An ordered composition mechanism that dispatches T to produce U.
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @note L1 mapping: Composite<T,U> — substrate for Pipeline<T> and Chain<T,U,P>.
template<typename T, typename U>
class Composite {
public:
    virtual U dispatch(T value) = 0;
};

/// @ingroup solidfi_l0
/// @brief A named, prioritized entry held inside a Composite.
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @note L1 mapping: Strategy<T,U> — the entry type inside Pipeline and Chain
///   (priority + name + impl). May be elevated to a named L1 concept in a future revision.
template<typename T, typename U>
class Strategy {
public:
    std::string name;
    int priority = 0;
    // impl: Operation<T> -- type TBD
};

/// @ingroup solidfi_l0
/// @brief Reserved. A directed relationship between two types T and U.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @todo Purpose TBD.
template<typename T, typename U>
class Goto {
public:
};

/// @ingroup solidfi_l0
/// @brief A type-safe chain of directed steps from T to U.
///
/// A Typechain is a composed sequence of Goto steps, each advancing the current type,
/// from source type T to destination type U through zero or more intermediate types.
/// The intermediate types are captured by the chain's structure but not named at this level.
///
/// Typechain is the single-chain form: no branching, one route. It is the structural
/// complement to Traversal: Traversal is the algorithm that walks a Graph; Typechain
/// is the explicit route through it.
///
/// @tparam T source type; start of the chain.
/// @tparam U destination type; end of the chain.
/// @note L1 mapping: Path<T,U,P> -- Typechain with Converter semantics and parameters.
template<typename T, typename U>
class Typechain {
public:
};

/// @ingroup solidfi_l0
/// @brief Captures a value of type T and produces it regardless of input.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Closed<T> — L1 names this Literal<T>. The alias Closed<T> is preserved
///   in the spec.
template<typename T>
class Closed {
public:
    explicit Closed(const T& value);
    virtual T get() const = 0;
};

/// @ingroup solidfi_l0
/// @reserved
/// @brief May or may not hold a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Optional<T> -- concretized as std::optional<T> in forward.hpp.
template<typename T>
class Optional {
public:
};

/// @ingroup solidfi_l0
/// @brief Shared ownership of a value of type T.
/// @tparam T the owned type; free generic, owned by the user.
/// @note L1 mapping: — L1 implementations may use std::shared_ptr<T> or equivalent.
///   Use Shared<T> in the spec to express that multiple holders own the same instance.
template<typename T>
class Shared {
public:
    virtual T& get() = 0;
};

/// @ingroup solidfi_l0
/// @brief A value of type T that cannot be modified after construction.
///
/// Named alias so the concept has an identity in the spec. In C++ this is `const T`.
/// In TypeScript this maps to `Readonly<T>`. Compatibility shims for each target
/// language live in their respective adapter layers.
///
/// @tparam T the underlying type; free generic, owned by the user.
/// @note L2 usage: Handshake::request is Readonly<R> — the original caller intent,
///   frozen at call time and never modified during traversal.
template<typename T>
using Readonly = const T;

/// @ingroup solidfi_l0
/// @brief A directed graph: typed nodes (types) and typed edges (converters).
///
/// In SolidFI, objects are types and arrows are `Converter<T,U>` instances — morphisms
/// between types. Graph is the concrete CS structure realizing that: nodes are types,
/// edges are Converter instances between them. Graph carries no SolidFI-specific
/// semantics — it is the raw structural substrate. The L1 projection (Domain) adds the
/// converter-aware install/remove API and the SolidFI identity.
///
/// @note Category theory provides the theoretical grounding, not the implementation —
///   there is nothing to Category beyond the CS structure Graph already names, so it is
///   an alias rather than a distinct type. See Category below.
/// @note L1 mapping: Domain — the SolidFI converter registry. Domain IS-A Graph.
class Graph {
public:
};

/// @ingroup solidfi_l0
/// @brief Category theory's name for Graph: objects (types) and arrows (converters).
///
/// Category is the mathematical vocabulary; Graph is its concrete CS realization. They
/// name the same structure, so Category is a plain alias — theoretical grounding, not a
/// second implementation.
using Category = Graph;

/// @ingroup solidfi_l0
/// @brief Reduces a collection of T to a single T (fold).
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: — no direct L1 counterpart yet.
template<typename T>
class Reduce {
public:
    virtual T reduce(std::vector<T> values) = 0;
};

/// @ingroup solidfi_l0
/// @brief Expands a single T to a collection of T (unfold).
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: — no direct L1 counterpart yet.
template<typename T>
class Expand {
public:
    virtual std::vector<T> expand(T value) = 0;
};

/// @ingroup solidfi_l0
/// @reserved
/// @brief Produces zero or more U values from a single T.
///
/// Fanout is the heterogeneous complement to Expand: where Expand produces T[]
/// from T (same type, structural split), Fanout produces U[] from T (different
/// type, value-based derivation). U need not be a structural part of T.
///
/// @tparam T source type.
/// @tparam U derived type; any value producible from T.
/// @note L1 mapping: — reserved for future L1 promotion.
template<typename T, typename U>
class Fanout {
public:
    virtual std::vector<U> fanout(T value) = 0;
};

} // namespace solidfi
