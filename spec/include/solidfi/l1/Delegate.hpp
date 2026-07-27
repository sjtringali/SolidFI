// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Delegate.hpp
/// @ingroup solidfi_l1_extras

#include "solidfi/l1/Converter.hpp"
#include <functional>

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @proposed
/// @brief Forwards to another Converter<T,U,P>, bound eagerly or built lazily.
///
/// Delegate<T,U,P> IS-A Converter<T,U,P> that holds a target converter and forwards
/// accepts()/rejects()/handles()/resolve() to it. The target can be supplied directly,
/// or built lazily on first use via a factory function — useful when constructing the
/// real converter is expensive and should be deferred until actually needed.
///
/// @note L2 mapping: Proxy, a boundary-crossing specialization of Delegate.
///
/// @tparam T source type; free generic, owned by the user.
/// @tparam U destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Delegate : public Converter<T, U, P> {
public:
    /// @brief Delegate to an existing converter, bound immediately.
    explicit Delegate(Converter<T, U, P> target);

    /// @brief Delegate to a converter built lazily on first use.
    /// @param factory Called at most once, the first time this Delegate is used,
    ///   to construct the target.
    explicit Delegate(std::function<Converter<T, U, P>()> factory);

    bool accepts(T value) const override;
    bool rejects(T value) const override;
    bool handles(P params) const override;

    /// @brief Forward to the target, building it first if it was constructed lazily.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) override;

private:
    Converter<T, U, P> delegate;
    std::function<Converter<T, U, P>()> factory;
};

} // namespace solidfi
