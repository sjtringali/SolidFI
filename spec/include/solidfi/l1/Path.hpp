// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Path.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/forward.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @accepted
/// @brief A sequential, type-advancing composition of Converters. IS-A Converter<T,U,P>.
///
/// Path is the heterogeneous composite over Converter: each step may advance the type
/// (T -> X -> Y -> U). Because Chain IS-A Converter, any step may itself be a Chain --
/// so branching is possible at any position. A Path where every step is a plain
/// Converter (no branching) produces a Trace at runtime; you cannot know this in
/// advance, only after traversal resolves.
///
/// To any caller holding a Converter<T,U,P> reference, a Path is indistinguishable from
/// a single converter by the Composite rule.
///
/// Relationship to Chain, Pipeline, Multipath, and Trace:
/// - Chain:      homogeneous Converter composite (T -> U at every step; routes via priority)
/// - Pipeline:   homogeneous Transform composite (T -> T at every step)
/// - Path:       heterogeneous Converter composite (T -> X -> Y -> U; steps may include Chain)
/// - Multipath:  construction helper that builds a Path with inline Chain/Pipeline sugar
/// - Trace:      the single unambiguous route actually taken through a Path at runtime;
///               only knowable after traversal. Reserved -- not yet defined.
///
/// Solver produces a Path<T,U,P>: a statically anchored T->U composition. Steps may
/// be plain Converters or Chains; branching at a step is resolved at runtime.
/// A Trace is what you get when you run it.
///
/// Steps are appended in execution order. Every step always runs, feeding its output
/// as the next step's input. There is no priority or routing at the Path level --
/// branching, if any, is delegated to a Chain installed at that step.
///
/// Failure policy follows Chain: Path holds a `Failed<U>` returned when the path is
/// empty. Each step is responsible for receiving and forwarding the failure value of
/// the previous step -- Path does not short-circuit on intermediate failure. Steps
/// MUST propagate failure: if step N returns a failure value, step N+1 must accept
/// it and return a failure value in turn.
///
/// **Invariants:**
/// - Steps run in append order. The output of step N is the input to step N+1.
/// - An empty Path returns its failed value immediately.
/// - Each step MUST propagate failure from the previous step.
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

    /// @brief Construct a Path from a variadic list of converters, in execution order.
    ///
    /// Each converter's output type must match the next converter's input type. With
    /// concepts, this is enforced at compile time. The first converter must accept T;
    /// the last must produce U. For short, fixed paths:
    /// @code
    ///   auto p = Path<Filename, HTML>::create(reader, unzipper, parser, renderer);
    /// @endcode
    template<typename... Converters>
    static Path<T, U, P> create(Converters&&... converters);

    /// @brief Append a converter as the next step in the path. Returns a new Path<T,I,P>.
    ///
    /// The returned path is independent of the receiver -- appending to it does not
    /// affect the original. The receiver may be discarded if only the extended path
    /// is needed. Any Converter<U,I,P> is accepted, including Chain.
    template<typename I>
    Path<T, I, P> append(Converter<U, I, P> converter);

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
