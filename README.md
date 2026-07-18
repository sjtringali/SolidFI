# SolidFI

SolidFI (**SOLID Functional Interfaces**) is a design specification for building composable systems out of small, strongly-typed, pluggable pieces.

## The Problem

Object-oriented design gives you names and structure, but usually locks composition in at design time. Pure functional design gives you fluid composition, but usually at the cost of types, names, and the sense of where one thing ends and another begins.

Determinism is a first-class value — because flexibility that makes behavior unpredictable isn't flexibility, so much as chaos with a nicer name.

Neither is wrong. But neither is complete.

## The Idea

Data is simple. What's interesting is what happens between data: the transitions, the transformations, the paths from one form to another. Most systems treat those transitions as second-class citizens: anonymous functions, implicit middleware, tangled pipelines.

SolidFI treats them as the primary thing. Transitions have names and types. Not because that's what makes composition possible, but because that's what makes it *legible*. You can read a composition and understand it. You can trace it when it breaks. You can reason about it as a typed system before anything runs. And when something goes wrong at runtime, you can see where it actually is, or at least have a better idea where to look.

## Composition is the Point

Small pieces that know only their own contract, assembled in explicit order, routing by self-declaration rather than external configuration. The structure is the configuration. Determinism is the default, non-determinism has to be earned.

Registries and discovery are real problems, and SolidFI doesn't pretend otherwise. It aims to express them in ways that are readable, typed, and composable rather than bolted on as an afterthought.

Bring your existing code. The interfaces are thin by design.

## Levels

**L1 is the primary surface.** Users write against L1. It defines the complete set of typed, named primitives for building compositions: how data flows, how it is converted, how change is represented, and how paths through a graph are found and executed.

**L0 is the theoretical substrate.** It names the foundational concepts that inform L1 — the mathematical and structural ideas that underlie the design. Implementers may find L0 useful as a reference layer; users generally don't need to. L0 and L1 are independent: L1 does not depend on L0, and names overlap intentionally where L1 concretizes an L0 concept.

**L2 is domain patterns.** Built on L1 primitives, not yet fully specified. Proxy, Runtime, Protocol, Transport, and others live here — concepts that are real and useful but need more definition before they belong in the core.

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

## Implementation Notes

While this spec is implemented in C++, it is only done so as a specification language to validate the concepts formally and produce the documentation. A full implementation in C++ or TypeScript is possible.

The spec and examples are written in an object-oriented style: classes, inheritance, explicit interfaces. This is intentional as a middle ground: it is readable in both C++ and TypeScript, and maps directly to working implementations in either language.

The OO style is not the end goal. The ideas map cleanly to C++ concepts and TypeScript's structural typing, where the inheritance hierarchy is replaced by `requires` constraints and duck typing respectively. Both are valid implementations of the same spec — the names, contracts, and composition rules stay identical. What changes is the mechanism: `implements Converter<T,U>` becomes `requires Converter<T,U>`, and the vtable disappears. The spec is written in OO to be concrete; it should be read as defining contracts, not mandating inheritance.

---

## Status

This is a personal spec, actively being developed. Reference implementation coming.

Not to be confused with Java's `@FunctionalInterface` — that's one interface with one method. This is a system of interfaces designed around composition. The plural is intentional.

## Author

Scott Tringali  
Copyright (c) 2026. All rights reserved.  
A permissive license will be applied upon publication.
