// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Deferred.hpp
/// @ingroup solidfi_l1_extras

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Transform.hpp"
#include "solidfi/l1/forward.hpp"
#include <functional>

namespace solidfi {

/// @ingroup solidfi_l1_extras
/// @proposed
/// @brief Lazy-constructing wrapper. The target is built on first use.
///
/// Deferred::Converter and Deferred::Transform are the two concrete forms.
/// Use Deferred::create<C>() to obtain one without naming the nested type:
///
/// @code
///   auto c = Deferred::create<MyConverter>();  // Deferred::Converter
///   auto t = Deferred::create<MyTransform>();  // Deferred::Transform
/// @endcode
///
/// Deferred is not itself instantiated; it is a scoping container for the
/// two nested types and their shared factory.
class Deferred {
public:
    Deferred() = delete;

    /// @brief Lazy Converter. Constructs the target on first resolve().
    ///
    /// @tparam T source type; free generic, owned by the user.
    /// @tparam U destination type; free generic, owned by the user.
    /// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
    template<typename T, typename U, typename P = Parameters>
    class Converter : public solidfi::Converter<T, U, P> {
    public:
        /// @brief Construct with a factory called at most once on first resolve().
        explicit Converter(std::function<solidfi::Converter<T, U, P>()> factory);

        /// @note Async-capable. Concrete implementations may execute asynchronously.
        U resolve(T value, P params) override;

    private:
        std::function<solidfi::Converter<T, U, P>()> factory_;
        Optional<solidfi::Converter<T, U, P>> instance_;
    };

    /// @brief Lazy Transform. Constructs the target on first apply().
    ///
    /// @tparam T value type; free generic, owned by the user.
    /// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
    template<typename T, typename P = Parameters>
    class Transform : public solidfi::Transform<T, P> {
    public:
        /// @brief Construct with a factory called at most once on first apply().
        explicit Transform(std::function<solidfi::Transform<T, P>()> factory);

        T apply(T value, P params) override;

    private:
        std::function<solidfi::Transform<T, P>()> factory_;
        Optional<solidfi::Transform<T, P>> instance_;
    };
    /// @brief Returns a Deferred::Converter that constructs C on first use.
    template<typename C>
    static auto create() -> Converter<typename C::source_type, typename C::target_type, typename C::params_type>;

    /// @brief Returns a Deferred::Transform that constructs C on first use.
    template<typename C>
    static auto create() -> Transform<typename C::value_type, typename C::params_type>;
};

} // namespace solidfi
