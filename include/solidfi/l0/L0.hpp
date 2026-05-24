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

namespace solidfi {  // NOLINT: intentional overlap with L1; include L0 standalone only

/// @brief A unit of executable work over a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Operation<T> — the core of Transform::apply and Converter::fetch.
template<typename T>
class Operation {
public:
    virtual void execute() = 0;
    virtual ~Operation() = default;
};

/// @brief A single binary decision over a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Predicate<T> — splits into accepts() and rejects() in L1, each
///   declared inline on Transform and Converter with default bodies.
template<typename T>
class Predicate {
public:
    virtual bool decide(T value) = 0;
    virtual ~Predicate() = default;
};

/// @brief A pair of filtering decisions over T: acceptance and explicit rejection.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Filter<T> — becomes the inline accepts()/rejects() methods declared
///   directly on Transform<T> and Converter<T,U,P>.
template<typename T>
class Filter {
public:
    virtual bool accepts(T value) = 0;
    virtual bool rejects(T value) = 0;
    virtual ~Filter() = default;
};

/// @brief An ordered composition mechanism that dispatches T to produce U.
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @note L1 mapping: Composite<T,U> — substrate for Pipeline<T> and Chain<T,U,P>.
template<typename T, typename U>
class Composite {
public:
    virtual U dispatch(T value) = 0;
    virtual ~Composite() = default;
};

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

/// @brief Captures a value of type T and produces it regardless of input.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Closed<T> — L1 names this Literal<T>. The alias Closed<T> is preserved
///   in the spec.
template<typename T>
class Closed {
public:
    explicit Closed(const T& value);
    virtual T get() const = 0;
    virtual ~Closed() = default;
};

/// @brief May or may not hold a value of type T.
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: Optional<T> — L1 aliases this to std::optional<T> in forward.hpp.
template<typename T>
class Optional {
public:
    virtual ~Optional() = default;
};

/// @brief Shared ownership of a value of type T.
/// @tparam T the owned type; free generic, owned by the user.
/// @note L1 mapping: — L1 implementations may use std::shared_ptr<T> or equivalent.
///   Use Shared<T> in the spec to express that multiple holders own the same instance.
template<typename T>
class Shared {
public:
    virtual T& get() = 0;
    virtual ~Shared() = default;
};

/// @brief Marker type for user-defined contextual data passed into Converter::fetch.
/// @note L1 mapping: Parameters — L1 concretizes this as a named empty struct, the default P
///   for all parameterized types.
class Parameters {
public:
    virtual ~Parameters() = default;
};

/// @brief Reduces a collection of T to a single T (fold).
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: — no direct L1 counterpart yet.
template<typename T>
class Reduce {
public:
    virtual T reduce(std::vector<T> values) = 0;
    virtual ~Reduce() = default;
};

/// @brief Expands a single T to a collection of T (unfold).
/// @tparam T source type; free generic, owned by the user.
/// @note L1 mapping: — no direct L1 counterpart yet.
template<typename T>
class Expand {
public:
    virtual std::vector<T> expand(T value) = 0;
    virtual ~Expand() = default;
};

} // namespace solidfi
