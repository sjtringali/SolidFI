#pragma once

/// @file groups.hpp
/// @brief Doxygen group definitions for the SolidFI level hierarchy.
///
/// This file contains only @defgroup declarations. Each concept file
/// uses @ingroup to assign itself to one of these groups.

/// @defgroup solidfi_l1 L1
/// @brief Level 1 — the core SolidFI primitives.
///
/// Fully specified abstractions for transformation, conversion, composition,
/// state management, and graph-based traversal.

/// @defgroup solidfi_l1_structural Structural
/// @ingroup solidfi_l1
/// @brief Core transformation and conversion primitives.
///
/// Defines how data flows through compositions: the shapes, contracts, and composites
/// that form the backbone of SolidFI. Includes Transform and Converter (the two abstract
/// roots), Pipeline and Chain (their composites), and specialized subtypes Generator,
/// Inverter, Provider, and Literal.

/// @defgroup solidfi_l1_state State
/// @ingroup solidfi_l1
/// @brief Primitives for representing and applying state changes.
///
/// Defines Delta<T> — the named, first-class representation of the change between two
/// states of T — and the Differencer and Applicator converters that produce and apply
/// deltas.

/// @defgroup solidfi_l1_runtime Runtime
/// @ingroup solidfi_l1
/// @brief Primitives for dynamic composition and graph-based traversal.
///
/// Defines Graph (the unordered registry of converter edges), Traversal (an algorithm
/// expressed as Converter<Graph,U,P>), and Solver (a Converter<T,U,P> that finds and
/// executes a path through a Graph).
