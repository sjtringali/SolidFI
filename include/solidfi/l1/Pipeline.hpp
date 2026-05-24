#pragma once

/// @file Pipeline.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Transform.hpp"
#include <string>

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief An ordered composition of Transform<T> instances. Is itself a Transform<T>.
///
/// To any caller holding a Transform<T> reference, a Pipeline is indistinguishable from
/// a single transform — the Composite rule. Pipeline is the composite form of Transform.
///
/// A rejected transform acts as a passthrough — the input is forwarded unchanged to the
/// next transform in priority order. Rejection is not failure; it is abstention.
///
/// Pipeline takes no parameters. If parameters are needed, use Chain<T,T,P> instead.
///
/// **Invariants:**
/// - A Pipeline MUST degrade to identity if no installed transform accepts the input.
/// - A rejected transform passes its input unchanged to the next transform in priority order.
/// - Names are group keys: multiple entries may share a name and are treated as a group.
///   remove(name) removes all entries with that name.
/// - Duplicate priorities result in undefined ordering within that priority level.
///
/// @tparam T source type; free generic, owned by the user.
template<typename T>
class Pipeline : public Transform<T> {
public:
    bool accepts(T value) const override;
    bool rejects(T value) const override;

    /// @brief Run the pipeline. Friendly alias for apply().
    ///
    /// Prefer run() when calling a known Pipeline directly.
    T run(T value);

    /// @brief Run all installed transforms in priority order.
    ///
    /// Rejected transforms are skipped; their input passes through unchanged.
    /// Degrades to identity if no transform accepts.
    T apply(T value) override;

    /// @brief Install a transform at the given priority under the given name.
    ///
    /// @param priority Determines execution order. Lower values run first.
    ///   Duplicate priorities result in undefined ordering within that level.
    /// @param name Group key. Multiple entries may share a name.
    /// @param transform The transform to install. May itself be a Pipeline<T>.
    void install(int priority, const std::string& name, Transform<T>& transform);

    /// @brief Remove all transforms with the given name.
    void remove(const std::string& name);

    /// @brief Replace the transform(s) with the given name, preserving priority.
    ///
    /// Equivalent to remove(name) followed by install() at the original priority.
    void replace(const std::string& name, Transform<T>& transform);
};

} // namespace solidfi
