#pragma once

/// @file Chain.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Transform.hpp"
#include <string>

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief An ordered composition of Converter instances. Is itself a Converter<T,U,P>.
///
/// To any caller holding a Converter<T,U,P> reference, a Chain is indistinguishable from
/// a single converter — the Composite rule. Chain is the composite form of Converter.
///
/// Execution order is determined by priority. The first converter that passes the filter
/// (rejects() is false AND accepts() is true) is attempted via fetch(). If fetch() succeeds,
/// the chain short-circuits and returns the result. If fetch() fails, the chain continues
/// to the next converter in priority order. If no converter succeeds, the chain fails.
///
/// **Filtering rule:**
/// @code
///   if (!rejects(value) && accepts(value)) → attempt fetch()
/// @endcode
/// accepts() and rejects() are an optimization — a converter with neither implemented is
/// always attempted. Selection is only final when fetch() succeeds.
///
/// A Chain may optionally have a prepare Transform<T> applied before dispatching to fetch(),
/// and a finalize `Transform<U>` applied after a successful fetch(). Either may be a Pipeline<T>
/// or `Pipeline<U>` — the composite rule ensures any Transform<T> satisfies the slot.
///
/// **Invariants:**
/// - Priority determines execution order. Duplicate priorities result in undefined ordering.
/// - Names are group keys: multiple entries may share a name. remove(name) removes all.
/// - If no converter succeeds, the chain fails (returns absent `Optional<U>`).
/// - prepare and finalize are optional; absent means no transform applied.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
///
/// @note Future: prepare and finalize may be hoisted to Graph as `Converter<T,T,P>` / `Converter<U,U,P>`
///   edges once P-driven selection is needed. Until then they live here.
template<typename T, typename U, typename P = Parameters>
class Chain : public Converter<T, U, P> {
public:
    /// @brief Construct an empty Chain with no prepare or finalize transform.
    Chain() = default;

    /// @brief Construct a Chain with optional prepare and finalize transforms.
    ///
    /// @param prepare  Transform applied to the input before dispatching to fetch(). May be nullptr.
    /// @param finalize Transform applied to the result after a successful fetch(). May be nullptr.
    explicit Chain(Transform<T>* prepare, Transform<U>* finalize = nullptr);

    /// @brief Optional transform applied to the input before dispatching to fetch().
    ///
    /// May be a Pipeline<T> — the composite rule ensures any Transform<T> satisfies this slot.
    /// Null means no prepare transform is applied.
    Transform<T>* prepare = nullptr;

    /// @brief Optional transform applied to the result after a successful fetch().
    ///
    /// May be a `Pipeline<U>` — the composite rule ensures any `Transform<U>` satisfies this slot.
    /// Null means no finalize transform is applied.
    Transform<U>* finalize = nullptr;

    bool accepts(T value) const override;
    bool rejects(T value) const override;

    /// @brief Execute the chain. Friendly alias for fetch().
    ///
    /// Prefer resolve() when calling a known Chain directly.
    Optional<U> resolve(T value, P params);

    /// @brief Try each installed converter in priority order until one succeeds.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    Optional<U> fetch(T value, P params) override;

    /// @brief Install a converter at the given priority under the given name.
    ///
    /// Accepts any Converter<T,U,P>, including another Chain, an Inverter, or a Solver.
    void install(int priority, std::string name, Converter<T, U, P> converter);

    /// @brief Remove all converters with the given name.
    void remove(std::string name);

    /// @brief Replace the converter(s) with the given name, preserving priority.
    ///
    /// Equivalent to remove(name) followed by install() at the original priority.
    void replace(std::string name, Converter<T, U, P> converter);
};

} // namespace solidfi
