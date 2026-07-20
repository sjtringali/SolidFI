// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file Graph.hpp
/// @ingroup solidfi_l1_compositions

#include "solidfi/l1/Path.hpp"
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Transform.hpp"

namespace solidfi {

/// @ingroup solidfi_l1_compositions
/// @proposed
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
/// @note L0 mapping: Category — objects are types, arrows are Converter edges.
/// @note L2 mapping: Runtime — a Graph assembled for a specific deployment context.
class Graph {
public:
    /// @brief Add a converter edge to the graph.
    template<typename T, typename U, typename P = Parameters>
    void install(Converter<T, U, P> converter);

    /// @brief Remove a converter edge from the graph.
    template<typename T, typename U, typename P = Parameters>
    void remove(Converter<T, U, P> converter);

    /// @brief Add a transform as a T→T edge.
    ///
    /// The Transform is adapted to a `Converter<T,T,Parameters>` at this boundary.
    /// The transform itself remains parameter-free; P-driven selection among T→T
    /// edges is handled by Solver during traversal.
    template<typename T>
    void install(Transform<T> transform);

    /// @brief Decompose a Path and install each step as an individual edge.
    ///
    /// The Path's explicit path is disassembled: each Converter and Transform is
    /// extracted and installed as if passed to install() directly. Steps whose identity
    /// already exists in the Graph are silently skipped. The Path itself is not
    /// installed as a single edge.
    ///
    /// This allows a statically-declared Path to seed the dynamic Graph without
    /// duplication. The Path may continue to be used directly alongside Solver.
    template<typename T, typename U, typename P = Parameters>
    void install(Path<T, U, P> blazer);

};

} // namespace solidfi
