#pragma once

/// @file Failure.hpp
/// @ingroup solidfi_l1_structural

namespace solidfi {

/// @ingroup solidfi_l1_structural
/// @brief Non-intrusive declaration of the failure sentinel for type U.
///
/// Specialize Failure<U> to declare which value of U represents a failed fetch().
/// Failure<U> is a Closed<U> — it holds a fixed value of U and returns it on demand
/// via a static value() method.
///
/// Chain uses Failure<U> to test whether a fetch() result represents failure:
/// @code
///   if (result == Failure<U>::value()) → fetch() failed; try next converter
/// @endcode
///
/// A type U that does not specialize Failure<U> cannot participate as the output
/// type of a Converter installed in a Chain. Converter itself has no such requirement.
///
/// The primary template is intentionally undefined. Using an unspecialized Failure<U>
/// in a Chain is a compile error — a deliberate participation contract.
///
/// Example:
/// @code
///   template<>
///   struct Failure<MyResult> {
///       static constexpr MyResult value() { return MyResult::none; }
///   };
///
///   // At the call site:
///   if (result == Failure<MyResult>::value()) { ... }
/// @endcode
///
/// @tparam U the type for which a failure sentinel is declared.
/// @note L0 mapping: Closed<T> — Failure<U> is a Closed<U> with semantic meaning.
/// @note Pattern: std::hash<T> — same non-intrusive specialization mechanism.
template<typename U>
struct Failure;

} // namespace solidfi
