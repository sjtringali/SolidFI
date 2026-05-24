# SolidFI

**Functional Interfaces for Composition**

SolidFI is a design spec for building composable systems out of small, typed, pluggable pieces. Determinism is a first-class value... because flexibility that makes behavior unpredictable isn't flexibility, so much as chaos with a nicer name.

## The Problem

Object-oriented design gives you names and structure, but usually locks composition in at design time. Pure functional design gives you fluid composition, but usually the cost of types, names, and the sense of where one thing ends and another begins.

Neither is wrong. But neither is complete.

## The Idea

Data is simple. What's interesting is what happens between data: the transitions, the transformations, the paths from one form to another. Most systems treat those transitions as second-class citizens: anonymous functions, implicit middleware, tangled pipelines.

SolidFI treats them as the primary thing. Transitions have names and types. Not because that's what makes composition possible, but because that's what makes it *legible*. You can read a composition and understand it. You can trace it when it breaks. You can reason about it as a typed system before anything runs. And when something goes wrong at runtime, you can see where it actually is, or at least have a better idea where to look.

## Composition is the Point

Small pieces that know only their own contract, assembled in explicit order, routing by self-declaration rather than external configuration. The structure is the configuration. Determinism is the default, non-determinism has to be earned.

Registries and discovery are real problems, and SolidFI doesn't pretend otherwise. It aims to express them in ways that are readable, typed, and composable rather than bolted on as an afterthought.

Bring your existing code. The interfaces are thin by design.

## Parameterized Traversal

**The Graph is your road network. The Solver is your GPS. A Proxy is a detour.**

The road network exists independently of any trip. You can ask whether a route between two points exists at all — that is a structural question about the roads, not about you or your destination. The Graph is that network: an unordered registry of converters, each one a segment connecting one type to another.

When you want to get somewhere, you ask the GPS. You tell it where you are (T), where you want to go (U), and optionally your preferences (P): fastest route, avoid highways, toll-free. The Solver finds a path through the Graph and drives it. P is the routing signal — same network, different preferences, different route. If a road is closed for your specific vehicle (`rejects`), the GPS routes around it.

Now imagine you are following the GPS route and you hit a road closure. A detour sign redirects you — not the GPS, not the map, just a sign on that specific road. Usually you follow it and rejoin the route; the GPS never had to recalculate. That is a Proxy. But a detour can also send you somewhere else entirely — a Proxy is not required to rejoin. It intercepts based on local conditions and can fully reroute, short-circuit, substitute a cached result, or redirect across a network boundary. The GPS had a plan; the road had other ideas.

Three levels:

1. **Static reachability** — is there any road at all between T and U? *(Graph structure; API not yet specified.)*
2. **Parameterized path finding** — given this T and these preferences P, find and drive the route. *(Solver)*
3. **Proxy traversal** — same route, but a detour sign on one road redirects you without touching the map. *(L2 Proxy)*

`prepare` and `finalize` on Chain are a normalization concern — the on-ramp and off-ramp of a specific segment, not a detour. They condition input before dispatch and output after fetch, unconditionally, every time that edge is used.

## Names for Important Things

SolidFI uses named marker types and named primitives so that every important concept has an identity. Anonymous types, functions, and compositions can make readability, debuggability, and reasoning challenging.

A name is not just a label — it is a place to attach relationships, invariants, and meaning. `Parameters` exists as a named marker so that the concept of "contextual data flowing into a conversion" has a home in the spec, even before its internal structure is defined.

If something matters, it has a name.

---

## Primitive Summary

### L0 — Substrate

Foundational concepts that inform L1. For implementers. L0 and L1 are independent.

| Concept          | Shape                      | Notes                                                                    |
| ------------------| ----------------------------| --------------------------------------------------------------------------|
| `Operation<T>`   | `execute()`                | Unit of executable work over T                                           |
| `Predicate<T>`   | `decide(T) -> bool`        | Single binary decision                                                   |
| `Filter<T>`      | `accepts(T)`, `rejects(T)` | Accept/reject pair                                                       |
| `Composite<T,U>` | `dispatch(T) -> U`         | Ordered composition substrate                                            |
| `Strategy<T,U>`  | name + priority            | Named, prioritized entry in a Composite                                  |
| `Delegate<T>`    | `target() -> T&`           | Object indirection: refers to another T and delegates to it. L2: `Proxy` |
| `Goto<T,U>`      | —                          | Reserved. Directed relationship T→U; purpose TBD                         |
| `Closed<T>`      | `get() -> T`               | Captures a T; produces it regardless of input. L1: `Literal<T>`          |
| `Optional<T>`    | —                          | May or may not hold a T. L1: `std::optional<T>`                          |
| `Shared<T>`      | `get() -> T&`              | Shared ownership of a T                                                  |
| `Parameters`     | —                          | Marker for user-defined contextual data                                  |
| `Category`       | —                          | Objects (types) + arrows (converters). L1: `Graph`. L2: `Runtime`        |
| `Reduce<T>`      | `reduce([T]) -> T`         | Fold: collection → single value                                          |
| `Expand<T>`      | `expand(T) -> [T]`         | Unfold: single value → collection                                        |

### L1 — Primitives

#### Structural Concerns

#### Core

| Concept            | Shape                | Notes                                                              |
| --------------------| ----------------------| --------------------------------------------------------------------|
| `Transform<T>`     | `apply(T) -> T`      | Takes T, produces T. Cannot fail; degrades to identity             |
| `Pipeline<T>`      | `run(T) -> T`        | Ordered composition of `Transform<T>`. Is itself a `Transform<T>`  |
| `Converter<T,U,P>` | `fetch(T,P) -> U?`   | Takes T, produces U. May fail. P for routing                       |
| `Chain<T,U,P>`     | `resolve(T,P) -> U?` | Ordered composition of `Converter`. Is itself a `Converter<T,U,P>` |
| `Parameters`       | empty                | Default P across all parameterized types                           |

##### Extras

| Concept             | Shape                                  | Notes                                                     |
| ---------------------| ----------------------------------------| -----------------------------------------------------------|
| `Generator<T,P>`    | `Converter<NoInput,T,P>`               | Produces T from nothing                                   |
| `Inverter<T,U>`     | `Converter<T,U>`                       | Guarantees bidirectional T↔U                              |
| `Provider<T,U>`     | `Converter<T,U>`                       | One-way lookup; I/O oriented                              |
| `Literal<T,InputT>` | `Transform<T>` + `Converter<InputT,T>` | Captures a T; satisfies both hierarchies. L0: `Closed<T>` |

#### State Concerns

| Concept          | Shape                       | Notes                                                     |
| ------------------| -----------------------------| -----------------------------------------------------------|
| `Delta<T>`       | marker                      | The change between two states of T; named and first-class |
| `Differencer<T>` | `Converter<T, Delta<T>, T>` | Current + previous state → delta                          |
| `Applicator<T>`  | `Converter<T, T, Delta<T>>` | Current state + delta → next state                        |

#### Runtime Concerns

| Concept          | Shape                     | Notes                                                      |
| ------------------| ---------------------------| ------------------------------------------------------------|
| `Graph`          | `install<T,U>` / `remove` | Unordered registry of Converter edges. Holds; does not act |
| `Traversal<U,P>` | `Converter<Graph,U,P>`    | Algorithm over a Graph; Graph is the input                 |
| `Solver<T,U,P>`  | `solve(T,P) -> U?`        | Graph bound at construction; finds and executes T→U path   |

### L2 — Domain Patterns

Stubs. Built on L1 primitives. Not yet fully specified.

| Concept         | Notes                                                          |
| -----------------| ----------------------------------------------------------------|
| `Protocol`      | Encoding and framing contract for a communication exchange     |
| `Transport`     | Physical or logical transmission channel                       |
| `Serialization` | Converter between in-memory representation and serialized form |
| `Proxy`         | Optional interception on a Converter; opt-in, default-off      |
| `Roundtrip`     | Bidirectional communication; builds on `Inverter<T,U>`         |
| `Runtime`       | Graph with dynamic plugin-loading machinery                    |


---

## Status

This is a personal spec, actively being developed. Reference implementation coming.

Not to be confused with Java's `@FunctionalInterface` — that's one interface with one method. This is a system of interfaces designed around composition. The plural is intentional.

## Author

Scott Tringali  
Copyright (c) 2026. All rights reserved.  
A permissive license will be applied upon publication.
