#pragma once

/// @file Delta.hpp
/// @ingroup solidfi_l1_state

namespace solidfi {

/// @ingroup solidfi_l1_state
/// @brief Represents the change between two states of T.
///
/// Delta is a named, first-class concept for the transition between states — not the
/// states themselves, but the difference between them. Making the transition explicit
/// and named gives it identity: it can be stored, inspected, applied, or reversed.
///
/// What a Delta<T> contains is defined by the user for their domain. The SolidFI
/// framework only names the concept; the structure is unconstrained.
///
/// @tparam T source type; free generic, owned by the user.
///
/// @see Differencer — produces a Delta<T> from two states of T.
/// @see Applicator — applies a Delta<T> to a state T to produce the next state.
template<typename T>
class Delta {
public:
    virtual ~Delta() = default;
};

} // namespace solidfi
