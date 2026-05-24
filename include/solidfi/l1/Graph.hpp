#pragma once

/// @file Graph.hpp
/// @ingroup solidfi_l1_runtime

#include "solidfi/l1/Converter.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_runtime
/// @brief An unordered, potentially cyclic registry of Converter edges.
///
/// Graph is a passive data structure — a bag of converter edges. Nodes are implied by
/// the type signatures of the installed converters; there are no explicit node objects.
/// Graph does not execute converters. Execution is Solver's concern.
///
/// Graph answers no runtime questions on its own. It holds; it does not act. Traversal
/// and Solver supply the verbs.
///
/// **Invariants:**
/// - Installed converters are unordered. No priority applies at the Graph level.
/// - The graph MAY contain cycles. Cycle detection and avoidance is Solver's responsibility.
/// - A Graph SHOULD be assembled at startup and treated as stable. It is not designed for
///   high-frequency mutation. Plugin loading is an L2 concern (see Runtime).
/// - Graph MUST NOT execute any converter.
///
/// @note L2: Runtime extends Graph with dynamic plugin-loading machinery.
class Graph {
public:
    /// @brief Add a converter edge to the graph.
    template<typename T, typename U, typename P = Parameters>
    void install(Converter<T, U, P>& converter);

    /// @brief Remove a converter edge from the graph.
    template<typename T, typename U, typename P = Parameters>
    void remove(Converter<T, U, P>& converter);

    virtual ~Graph() = default;
};

} // namespace solidfi
