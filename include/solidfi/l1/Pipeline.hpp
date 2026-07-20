// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Pipeline.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Parameters.hpp"
#include "solidfi/l1/Transform.hpp"
#include <string>

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @accepted
/// @brief An ordered composition of Transform<T,P> instances. Is itself a Transform<T,P>.
///
/// To any caller holding a Transform<T,P> reference, a Pipeline is indistinguishable from
/// a single transform — the Composite rule. Pipeline is the composite form of Transform.
///
/// A skipped transform (rejects() or !handles()) acts as a passthrough — the input is
/// forwarded unchanged to the next transform in priority order. Skipping is not failure;
/// it is abstention. Pipeline MUST degrade to identity if no transform runs.
///
/// **Invariants:**
/// - A Pipeline MUST degrade to identity if no installed transform accepts the input.
/// - A skipped transform passes its input unchanged to the next in priority order.
/// - Names are group keys: multiple entries may share a name and are treated as a group.
///   remove(name) removes all entries with that name.
/// - Duplicate priorities are rejected — install() MUST fail (throw or return an error)
///   rather than silently produce undefined ordering.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename P = Parameters>
class Pipeline : public Transform<T, P> {
public:
    bool accepts(T value) const override;
    bool rejects(T value) const override;
    bool handles(P params) const override;

    /// @brief Run the pipeline. Friendly alias for apply().
    ///
    /// Prefer run() when calling a known Pipeline directly.
    T run(T value, P params);

    /// @brief Run all installed transforms in priority order.
    ///
    /// Skipped transforms pass their input through unchanged.
    /// Degrades to identity if no transform runs.
    T apply(T value, P params) override;

    /// @brief Install a transform at the given priority under the given name.
    ///
    /// @param priority Determines execution order. Lower values run first.
    ///   Duplicate priorities are rejected — install() MUST fail rather than silently reorder.
    /// @param name Group key. Multiple entries may share a name.
    /// @param transform The transform to install. May itself be a Pipeline<T,P>.
    void install(int priority, std::string name, Transform<T, P> transform);

    /// @brief Remove all transforms with the given name.
    void remove(std::string name);

    /// @brief Replace the transform(s) with the given name, preserving priority.
    ///
    /// Equivalent to remove(name) followed by install() at the original priority.
    void replace(std::string name, Transform<T, P> transform);
};

} // namespace solidfi
