#pragma once

/// @file groups.hpp

/// @defgroup solidfi_l1 L1
/// @brief The primary surface — fully specified interfaces for composable systems.
/// L1 is what you use. It defines the complete set of typed, named primitives for
/// building compositions: how data flows and is converted (Structural), how change is
/// represented and applied (State), and how paths through a graph are found and executed
/// (Runtime).

/// @defgroup solidfi_l1_structural Structural
/// @ingroup solidfi_l1
/// @brief Transformation and conversion — the shapes data takes and the paths it travels.
/// Transform (T → T, cannot fail) and Converter (T → U, may fail, carries P) are the two
/// abstract roots. Pipeline and Chain are their composites — each satisfies the Composite
/// rule and is itself a Transform or Converter. Generator, Inverter, Provider, and Literal
/// are specialized subtypes.

/// @defgroup solidfi_l1_state State
/// @ingroup solidfi_l1
/// @brief Change as a first-class value.
/// Delta<T> names the difference between two states of T. Differencer produces a Delta
/// from a current and previous state; Applicator consumes one to advance state. State
/// transitions get an explicit, typed identity rather than being buried in mutation.

/// @defgroup solidfi_l1_runtime Runtime
/// @ingroup solidfi_l1
/// @brief Graph-based traversal — the road network, the GPS, and the route.
/// Graph is the registry: an unordered bag of Converter edges, passive, assembled at
/// startup. Solver is the engine: given a starting type T and parameters P, it finds and
/// executes a path to U through the Graph. Traversal is the abstract form of that
/// algorithm — a Converter whose input is the Graph itself.
