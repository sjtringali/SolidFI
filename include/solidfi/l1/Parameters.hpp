#pragma once

/// @file Parameters.hpp
/// @ingroup solidfi_l1_structural


namespace solidfi {

/// @ingroup solidfi_l1_structural

/// @accepted
/// @brief Named marker type for user-defined contextual data passed into Converter::fetch.
///
/// Parameters is the default type for the P template parameter across all SolidFI primitives
/// that accept routing data: Converter, Chain, Generator, Solver, Traversal.
///
/// The framework never inspects or constrains Parameters — it only passes it through to fetch().
/// Users may substitute any type for P; this empty struct is the named default.
///
/// Parameters is not just "context" — it is the routing signal that drives parameterized
/// traversal at every level: which converter the chain selects, how predicates filter,
/// which path a Solver takes through the Graph.
///
/// P is always passed by value. If shared ownership is needed, make P itself a pointer or
/// a shared-ownership wrapper — e.g. `P = MyParams*` or `P = Shared<MyParams>` (see L0).
/// The framework copies P as-is; pointer semantics transfer naturally.
///
/// @note L0 mapping: Parameters — named at L0 as the routing concept; concretized here.
struct Parameters {};

} // namespace solidfi
