// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Path.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/forward.hpp"
#include <initializer_list>

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @accepted
/// @brief A sequential, type-advancing composition of Converters. IS-A Converter<T,U,P>.
///
/// Path is the heterogeneous composite over Converter: each step may advance the type
/// (T -> X -> Y -> U), unlike Chain where every link shares T and U. To any caller
/// holding a Converter<T,U,P> reference, a Path is indistinguishable from a single
/// converter by the Composite rule.
///
/// Relationship to Chain and Pipeline:
/// - Chain:    homogeneous Converter composite (T -> U at every step; routes via priority)
/// - Pipeline: homogeneous Transform composite (T -> T at every step)
/// - Path:     heterogeneous Converter composite (T -> X -> Y -> U; every step always runs)
///
/// Steps are appended in execution order. There is no priority or routing -- every step
/// always runs, feeding its output as the next step's input. append() is the only
/// mutation; there is no insert or remove.
///
/// Failure policy follows Chain: Path holds a `Failed<U>` returned when the path is
/// empty. Each step's own failure semantics are its own responsibility; Path does not
/// check intermediate results.
///
/// **Invariants:**
/// - Steps run in append order. The output of step N is the input to step N+1.
/// - An empty Path returns its failed value immediately.
/// - Path MUST NOT modify any Converter it holds.
///
/// @tparam T source (start) type; input type of the first step.
/// @tparam U destination (end) type; output type of the last step.
/// @tparam P parameters type; flows into every Converter along the path. Defaults to Parameters.
template<typename T, typename U, typename P = Parameters>
class Path : public Converter<T, U, P> {
public:
    /// @brief Construct an empty Path with an optional failure policy.
    /// @param failed Value returned when the path is empty.
    ///   Defaults to `Failed<U>{}`, which zero-initializes value (null for pointer and nullable types).
    Path(Failed<U> failed = Failed<U>{});

    /// @brief Construct a Path from an initializer list of converters, in execution order.
    ///
    /// Equivalent to default-constructing and appending each converter in order.
    /// For short, fixed paths:
    /// @code
    ///   auto p = Path<Filename, HTML>::create({reader, unzipper, parser, renderer});
    /// @endcode
    static Path<T, U, P> create(std::initializer_list<Converter<T, U, P>> converters);

    /// @brief Append a converter as the next step in the path.
    ///
    /// The converter's input type must match the current end type; this is a
    /// caller responsibility -- the path does not verify intermediate types.
    void append(Converter<T, U, P> converter);

    /// @brief Execute the path. Friendly alias for resolve().
    ///
    /// Prefer traverse() when calling a known Path directly.
    U traverse(T value, P params) noexcept;

    /// @note Async-capable. Concrete implementations may execute asynchronously.
    U resolve(T value, P params) noexcept override;

private:
    Failed<U> pathFailed;
};

} // namespace solidfi
