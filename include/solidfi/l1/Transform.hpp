#pragma once

/// @file Transform.hpp
/// @ingroup solidfi_l1_structural

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Takes a value of type T and produces a value of type T.
///
/// Input and output are the same type. A Transform cannot fail — it MUST return the
/// original T if change is not possible. This is the degradation floor.
///
/// Transforms do not take parameters. If contextual parameters are needed, use
/// Converter<T,T,P> instead — the presence of P signals that the concern is conversion,
/// not transformation.
///
/// A Transform that does not accept its input MUST return the original value unchanged.
///
/// **Invariants:**
/// - accepts() and rejects() are stateless, synchronous, and free of side effects.
/// - apply() MUST return a value of type T.
/// - A transform that does not accept its input MUST return the original value unchanged.
///
/// @tparam T source type; free generic, owned by the user.
template<typename T>
class Transform {
public:
    /// @brief Returns true if this transform claims the input. Default: true.
    ///
    /// Callers are NOT required to call accepts() — it is a fast-path optimization to
    /// avoid calling apply() when the outcome is already known.
    virtual bool accepts(T value) const { return true; }

    /// @brief Returns true if this transform explicitly refuses the input. Default: false.
    ///
    /// Evaluated before accepts(). A transform that rejects is skipped entirely and
    /// its apply() is never called. Rejection is not failure; it is abstention.
    virtual bool rejects(T value) const { return false; }

    /// @brief Perform the transformation. MUST return a T.
    ///
    /// If this transform does not accept the input, MUST return the original value unchanged.
    virtual T apply(T value) = 0;

    virtual ~Transform() = default;
};

} // namespace solidfi
