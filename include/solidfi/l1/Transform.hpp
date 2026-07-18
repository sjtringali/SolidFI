#pragma once

/// @file Transform.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Parameters.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Takes a value of type T and produces a value of type T.
///
/// Input and output are the same type. A Transform cannot fail — it MUST return the
/// original T if change is not possible. This is the degradation floor.
///
/// P flows through from the caller. A Transform that does not care about P never
/// overrides handles() and runs unconditionally. A Transform that does override
/// handles() is skipped by Pipeline when handles() returns false — same
/// skip-as-passthrough semantics as rejects(). Opting in to P requires no changes
/// to existing Transforms; P = Parameters is empty by default.
///
/// **Filtering rule** (shared with Pipeline):
/// @code
///   if (accepts(value) && !rejects(value) && handles(params)) → call apply()
/// @endcode
/// All three must pass; evaluation order is unspecified and implementation-defined.
///
/// **Invariants:**
/// - accepts(), rejects(), and handles() are stateless, synchronous, and side-effect-free.
/// - accepts() and rejects() MUST NOT depend on P.
/// - handles() MUST NOT depend on T.
/// - apply() MUST return a value of type T.
/// - A transform that does not accept its input MUST return the original value unchanged.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename P = Parameters>
class Transform {
public:
    /// @brief Returns true if this transform claims the input. Default: true.
    virtual bool accepts(T value) const { return true; }

    /// @brief Returns true if this transform explicitly refuses the input. Default: false.
    ///
    /// Evaluated before accepts(). A transform that rejects is skipped; apply() is never
    /// called. Rejection is not failure; it is abstention.
    virtual bool rejects(T value) const { return false; }

    /// @brief Returns true if this transform can handle these parameters. Default: true.
    ///
    /// Evaluated after accepts(). A transform that does not handle the parameters is
    /// skipped by Pipeline — same passthrough semantics as rejects().
    /// MUST NOT depend on T — that belongs in accepts().
    virtual bool handles(P params) const { return true; }

    /// @brief Perform the transformation. MUST return a T.
    ///
    /// If this transform does not accept the input, MUST return the original value unchanged.
    virtual T apply(T value, P params) = 0;

};

} // namespace solidfi
