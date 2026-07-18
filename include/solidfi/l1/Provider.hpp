#pragma once

/// @file Provider.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @proposed
/// @brief A Converter oriented toward one-way lookups, typically involving external data sources.
///
/// Provider implies a one-directional fetch from an external resource — I/O, a database,
/// a network service, a registry. Unlike Inverter, there is no reverse direction guarantee.
///
/// @tparam T source type (lookup key); free generic, owned by the user.
/// @tparam U destination type (looked-up value); free generic, owned by the user.
///
/// @todo Full specification pending.
template<typename T, typename U>
class Provider : public Converter<T, U> {
public:
    /// @brief Perform the lookup. Returns absent value if the lookup fails or produces nothing.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U fetch(T value, Parameters params) override = 0;
};

} // namespace solidfi
