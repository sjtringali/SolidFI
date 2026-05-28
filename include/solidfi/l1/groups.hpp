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
/// @brief The atomic primitives — the roots everything else composes from.
/// Transform (T → T, cannot fail) and Converter (T → U, may fail, carries P) are the two
/// abstract roots. Parameters and Sentinel complete the core contract.
/// Generator, Inverter, Provider, and Literal are specialized subtypes.

/// @defgroup solidfi_l1_state State
/// @ingroup solidfi_l1
/// @brief Change as a first-class value.
/// Delta<T> names the difference between two states of T. Differencer produces a Delta
/// from a current and previous state; Applicator consumes one to advance state. State
/// transitions get an explicit, typed identity rather than being buried in mutation.

/// @defgroup solidfi_l1_compositions Compositions
/// @ingroup solidfi_l1
/// @brief Static and dynamic composition — how primitives are combined into larger wholes.
/// Pipeline and Chain are the composite forms of Transform and Converter. Path wires
/// an explicit multi-stage path at construction. Graph is the dynamic registry of edges;
/// Solver traverses it at runtime; Router pins a specific T→U path as a Converter.
/// Registry is the general-purpose runtime complement to Extensible.
