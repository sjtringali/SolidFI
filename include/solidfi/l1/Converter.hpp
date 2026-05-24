#pragma once

/// @file Converter.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/forward.hpp"
#include "solidfi/l1/Parameters.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Takes a value of type T and produces a value of type U.
///
/// T and U are typically different types — this is a genuine conversion, not a
/// transformation. Because T and U are distinct, failure is possible: there is no
/// identity fallback. Failure is represented by an absent `Optional<U>`.
///
/// P is an optional user-defined parameter type for routing and dispatch. The framework
/// never inspects P — it only passes it through to fetch(). Defaults to Parameters.
///
/// **Filtering rule** (shared with Chain):
/// @code
///   if (!rejects(value) && accepts(value)) → attempt fetch()
/// @endcode
/// accepts() and rejects() are optimizations — a converter with neither implemented is
/// always attempted via fetch(). They MUST be stateless, synchronous, and side-effect-free.
/// They MUST NOT depend on P; that logic belongs in fetch().
///
/// **Invariants:**
/// - accepts() and rejects() MUST be stateless and synchronous.
/// - accepts() and rejects() MUST NOT depend on P.
/// - fetch() MAY fail; returns absent `Optional<U>` on failure.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Converter : public ConverterBase {
public:
    /// @brief Returns true if this converter claims the input. Default: true.
    virtual bool accepts(const T& value) const { return true; }

    /// @brief Returns true if this converter explicitly refuses the input. Default: false.
    ///
    /// Evaluated before accepts(). A converter that rejects is never attempted via fetch().
    virtual bool rejects(const T& value) const { return false; }

    /// @brief Perform the conversion. Returns absent value on failure.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    virtual `Optional<U>` fetch(const T& value, const P& params) = 0;

    std::type_index sourceType() const override { return typeid(T); }
    std::type_index targetType() const override { return typeid(U); }

    virtual ~Converter() = default;
};

} // namespace solidfi
