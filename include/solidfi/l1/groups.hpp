// Copyright (c) 2026 Scott Tringali. All rights reserved.

#pragma once

/// @file groups.hpp

/// @defgroup solidfi_l1 L1
/// @brief The primary surface — fully specified interfaces for composable systems.
/// L1 is what you use. It defines the complete set of typed, named primitives for
/// building compositions: how data flows and is converted (Core), how change is
/// represented and applied (State), and how things are composed statically and
/// dynamically (Compositions).

/// @defgroup solidfi_l1_structural Core
/// @ingroup solidfi_l1
/// @brief The primary primitives — the roots everything else composes from.
/// Transform (T → T, cannot fail) and Converter (T → U, may fail, carries P) are the two
/// abstract roots. Parameters and Failed complete the core contract.
/// Chain and Pipeline are the composite forms of Converter and Transform respectively —
/// ordered compositions that are themselves Converter and Transform, enabling the Composite rule.
/// Generator, Inverter, Provider, and Literal are specialized subtypes.

/// @defgroup solidfi_l1_state State
/// @ingroup solidfi_l1
/// @brief Change as a first-class value.
/// Delta<T> names the difference between two states of T. Differencer produces a Delta
/// from a current and previous state; Applicator consumes one to advance state. State
/// transitions get an explicit, typed identity rather than being buried in mutation.

/// @defgroup solidfi_l1_compositions Compositions
/// @ingroup solidfi_l1
/// @brief The generalized graph solver — static paths and dynamic traversal.
/// Path wires an explicit multi-stage route at construction time; IS-A Converter<T,U,P>.
/// Graph is the untyped registry of Converter edges.
/// Solver<T,U,P> IS-A Converter<Graph,Path<T,U,P>,P> — typed discovery for compile-time-known T and U.
/// Pathfinder is the untyped complement: Graph-bound, one instance, any T->U query at runtime.
/// Router<T,U,P> composes either with Path traversal as a single Converter<T,U,P>: find-and-execute.
/// Traversal<U> is the reserved abstract base for traversal algorithms over a Graph.
/// Registry is the general-purpose runtime complement to Extensible.
