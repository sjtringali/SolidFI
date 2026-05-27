#pragma once

/// @file L0.hpp
/// @brief SolidFI Level 0 — substrate concepts. Named stubs only; not implemented here.
///
/// L0 defines the foundational primitives that inform L1. Each concept is declared as a
/// stub with a note describing its relationship to L1. L0 and L1 are independent — L1
/// does not include or depend on L0. Do not include this alongside L1 headers; names
/// intentionally overlap (e.g. Optional, Parameters) since L1 concretizes L0 concepts.

#include <string>
#include <vector>

/// @defgroup solidfi_l0 L0
/// @brief The substrate — named abstract concepts that underlie L1.
/// L0 defines the vocabulary of the spec without prescribing implementation. Each concept
/// is a stub: it claims an identity and notes its relationship to L1, but imposes no
/// structure on how it is realized. L0 is for implementers — a reference layer that names
/// what L1 concretizes. L0 and L1 are independent; names overlap intentionally.

namespace solidfi {  // NOLINT: intentional overlap with L1; include L0 standalone only

/// @ingroup solidfi_l0
/// @brief A unit of executable work over a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Operation<T> — the core of Transform::apply and Converter::fetch.
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
/// @brief Object indirection: refers to another T and delegates to it.
///
/// Delegate<T> is the abstract concept of "this T points to another T." It carries no
/// network assumptions, no lifecycle policy — just the redirect. Any T that holds a
/// Delegate<T> can delegate its behavior to the target instead of executing directly.
///
/// At L1 a Converter may carry a Delegate as a field (creation-bound or injected), making
/// it either a delegating converter or a passthrough to another. At L2 this concept is
/// specialized into Proxy, which adds network transparency, live service directories,
/// and remote redirection.
///
/// @tparam T the type being redirected; free generic, owned by the user.
/// @note L1 mapping: optional redirect field on Converter — TBD.
/// @note L2 mapping: Proxy — the network-transparency specialization of Delegate.
template<typename T>
class Delegate {
public:
    virtual T& target() = 0;
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
/// @brief May or may not hold a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Optional<T> — L1 aliases this to std::optional<T> in forward.hpp.
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
/// @brief Marker type for user-defined contextual data passed into Converter::fetch.
/// @note L1 mapping: Parameters — L1 concretizes this as a named empty struct, the default P
///   for all parameterized types.
class Parameters {
public:
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
/// @brief A collection of objects (types) and arrows (converters) between them.
///
/// In SolidFI, objects are types and arrows are `Converter<T,U>` instances —
/// morphisms between types. Category is the abstract substrate concept; it names the
/// mathematical structure that the framework implements.
///
/// @note L1 mapping: Graph — the concrete unordered registry of Converter edges.
/// @note L2 mapping: Runtime — a Graph assembled for a specific deployment context.
class Category {
public:
};

/// @ingroup solidfi_l0
/// @brief An algorithm that accepts a Category and produces a result of type U.
///
/// The abstract shape of graph traversal: Category in, typed result out. U may be
/// a reachability bool, an executed conversion result, or any other type produced
/// by walking the graph.
///
/// @tparam U destination type; the result produced by this traversal.
/// @note L1 mapping: Solver - binds a Graph at construction rather than accepting it as input.
template<typename U>
class Traversal {
public:
    virtual U traverse(Category graph) = 0;
};

/// @ingroup solidfi_l0
/// @brief Reserved. A found path through a Category from one type to another.
///
/// Represents the concrete result of path-finding: an ordered sequence of arrows
/// (converters) connecting one object (type) to another. Distinct from Traversal,
/// which is the algorithm; this is what the algorithm finds.
///
/// @todo Name TBD. Candidates: Path, Trail, Track.
class FoundPath {
};

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

} // namespace solidfi
