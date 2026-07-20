#pragma once

/// @file solidfi_l1.hpp
/// @brief SolidFI Level 1 — master include. Brings in all L1 concepts.

// Foundation
#include "solidfi/l1/forward.hpp"
#include "solidfi/l1/Parameters.hpp"
#include "solidfi/l1/groups.hpp"

// Structural
#include "solidfi/l1/Sentinel.hpp"
#include "solidfi/l1/Transform.hpp"
#include "solidfi/l1/Converter.hpp"
#include "solidfi/l1/Pipeline.hpp"
#include "solidfi/l1/Chain.hpp"
#include "solidfi/l1/Path.hpp"
#include "solidfi/l1/Registry.hpp"
#include "solidfi/l1/Generator.hpp"
#include "solidfi/l1/Inverter.hpp"
#include "solidfi/l1/Provider.hpp"
#include "solidfi/l1/Literal.hpp"

// State
#include "solidfi/l1/Delta.hpp"
#include "solidfi/l1/Differencer.hpp"
#include "solidfi/l1/Applicator.hpp"

// Runtime
#include "solidfi/l1/Graph.hpp"
#include "solidfi/l1/Pathfinder.hpp"
#include "solidfi/l1/Solver.hpp"
#include "solidfi/l1/Router.hpp"
