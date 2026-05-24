#pragma once

/// @file Generator.hpp
/// @ingroup solidfi_l1_structural

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Produces a value of type T from nothing.
///
/// Generator is conceptually Converter<void, T, P> — a conversion with no meaningful input.
/// Because void is not a valid C++ template type argument, the input type is modeled as
/// NoInput, a tag type carrying no data. Callers pass NoInput{} as the input value.
///
/// @tparam T destination type; free generic, owned by the user.
/// @tparam P parameters type; named marker, mostly user-owned. Defaults to Parameters.
template<typename T, typename P = Parameters>
class Generator : public Converter<NoInput, T, P> {
public:
    bool accepts(NoInput) const override { return true; }
    bool rejects(NoInput) const override { return false; }

    /// @brief Produce a T. The NoInput argument carries no data and should be ignored.
    ///
    /// @note Async-capable. Concrete implementations may execute asynchronously.
    Optional<T> fetch(NoInput input, P params) override = 0;
};

} // namespace solidfi
