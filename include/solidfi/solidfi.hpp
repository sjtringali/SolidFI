#pragma once

/// @file solidfi.hpp
/// @brief SolidFI — master include for all levels.
///
/// Including this file brings in all L1 and L2 SolidFI concepts.
/// For finer-grained inclusion, use the level-specific headers directly:
/// - L0: solidfi/l0/L0.hpp  (standalone only — conflicts with L1 names by design)
/// - L1: solidfi/l1/solidfi_l1.hpp
/// - L2: solidfi/l2/L2.hpp

#include "solidfi/l1/solidfi_l1.hpp"
#include "solidfi/l2/L2.hpp"
