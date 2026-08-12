// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Chain.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Transform.hpp"
#include "solidfi/l1/forward.hpp"
#include <string>

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @accepted{8/4/26}
/// @brief Failure policy for Chain: holds the sentinel value returned when no converter succeeds.
///
/// Chain accepts a `Failed<U>` rather than a raw U so that the sentinel can be provided
/// by any U-producing concept (Provider, Literal, Generator, etc.) that exposes a `value`
/// member. Structural compatibility is sufficient; no explicit relationship is required.
///
/// The default implementation returns null (suitable for pointer and nullable types).
/// For non-nullable U, construct an explicit `Failed<U>` and pass it to the Chain constructor.
///
/// @tparam U destination type of the owning Chain.
template<typename U>
struct Failed {
    U value;
};

/// @ingroup solidfi_l1_structural
/// @accepted{5/24/26}
/// @brief An ordered composition of Converter instances. Is itself a Converter<T,U,P>.
///
/// To any caller holding a Converter<T,U,P> reference, a Chain is indistinguishable from
/// a single converter by the Composite rule. Chain is the composite form of Converter.
///
/// Execution order is determined by priority. The first converter that passes the filter
/// (rejects() is false AND accepts() is true) is attempted via resolve(). If the result is
/// not equal to the applicable failure value, the chain short-circuits and returns it.
/// Otherwise the chain continues to the next converter in priority order. If no converter
/// succeeds, the chain fails and returns its own failed value.
///
/// Each link may declare its own failure value via the four-argument install() overload.
/// A link installed without one is compared against the chain's own failed value
/// instead (see the Chain constructor).
///
/// **Filtering rule:**
/// @code
///   if (accepts(value) && !rejects(value) && handles(params)) → attempt resolve()
/// @endcode
/// All three must pass; evaluation order is unspecified and implementation-defined.
/// A converter with none overridden is always attempted.
/// Selection is only final when resolve() succeeds.
///
/// Chain has its own prepare Transform<T> and finalize Transform<U> by definition
/// (inherited from Converter, not redeclared here), conditioning the input and output
/// of the whole chain. Either may be a Pipeline<T> or `Pipeline<U>`, as the composite rule
/// ensures any Transform<T> satisfies the slot.
///
/// Because every installed link is itself a Converter, a link may carry its own prepare
/// and finalize too. Chain calls them: a link's prepare (if set) conditions the value
/// immediately before that link's resolve() is attempted, and its finalize (if set)
/// conditions the value immediately after. A null prepare or finalize, whether Chain's
/// own or a link's, means no change, not "stop dispatch": resolve() is still attempted
/// normally, only the missing conditioning step is skipped.
///
/// prepare and finalize are normalization, not routing: the Solver
/// does not see them, P does not select them, and they apply on every traversal of this
/// Chain regardless of path. Normalization that belongs to this Chain lives here;
/// normalization that should be visible across the Graph goes in via Graph::install(Transform<T>).
///
/// For dynamic interception of a converter from the outside, injecting behavior without
/// modifying the Graph, see Proxy (L2).
///
/// **Invariants:**
/// - Priority determines execution order. Duplicate priorities are rejected: install() MUST
///   fail (throw or return an error) rather than silently produce undefined ordering.
/// - Names are group keys: multiple entries may share a name. remove(name) removes all.
/// - If no converter succeeds, the chain fails and returns its own configured
///   failed value (see the `failed` constructor parameter).
/// - A link's failure value, if given at install(), is what that link's resolve()
///   is compared against. Omitted means the chain's own failed value applies.
/// - prepare and finalize (Chain's own, inherited from Converter, and each link's own)
///   are optional; null means no change, not "stop dispatch": resolve() still runs,
///   only the missing conditioning step is skipped.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Chain : public Converter<T, U, P> {
public:
    /// @brief Construct a Chain with an optional failure policy.
    /// @param failed Failure policy; its value member is returned when no converter succeeds.
    ///   Defaults to `Failed<U>{}`, which zero-initializes value (null for pointer and nullable types).
    ///
    /// Any U-producing concept that exposes a `value` member satisfies this parameter
    /// structurally (Provider, Literal, Generator, etc.). No explicit relationship required.
    Chain(Failed<U> failed = Failed<U>{});

    /// @brief Construct a Chain with optional prepare and finalize transforms.
    ///
    /// Sets the prepare and finalize members inherited from Converter.
    ///
    /// @param prepare  Transform that conditions the input. May be nullptr.
    /// @param finalize Transform that conditions the result. May be nullptr.
    /// @accepted
    explicit Chain(Transform<T>* prepare, Transform<U>* finalize = nullptr);

    bool accepts(T value) const noexcept override;
    bool rejects(T value) const noexcept override;

    /// @brief Execute the chain. Friendly alias for resolve().
    ///
    /// Prefer dispatch() when calling a known Chain directly.
    U dispatch(T value, P params) noexcept;

    /// @brief Try each installed converter in priority order until one succeeds.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) noexcept override;

    /// @brief Install a converter at the given priority under the given name.
    ///
    /// Accepts any Converter<T,U,P>, including another Chain, an Inverter, or a Solver.
    /// This link's resolve() is compared against the chain's own failed value.
    void install(Priority priority, std::string name, Converter<T, U, P> converter);

    /// @brief Install a converter with its own per-link failure value.
    ///
    /// Accepts any Converter<T,U,P>, including another Chain, an Inverter, or a Solver.
    ///
    /// @param failed Per-link failure value of U. This link's resolve() is compared
    ///   against it, instead of the chain's own failed value, to decide whether this
    ///   link failed.
    void install(Priority priority, std::string name, Converter<T, U, P> converter, U failed);

    /// @brief Remove all converters with the given name.
    void remove(std::string name);

    /// @brief Replace the converter(s) with the given name, preserving priority.
    /// @proposed
    ///
    /// Equivalent to remove(name) followed by install() at the original priority.
    void replace(std::string name, Converter<T, U, P> converter);

    /// @brief Replace the converter(s) with the given name, preserving priority,
    /// with its own per-link failure value.
    /// @proposed
    ///
    /// Equivalent to remove(name) followed by install() at the original priority.
    void replace(std::string name, Converter<T, U, P> converter, U failed);

private:
    Failed<U> chainFailed;
};

} // namespace solidfi
