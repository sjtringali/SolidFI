# SolidFI

SolidFI (**SOLID Implmentation for Functional Interfaces**) is a typed interface specification for composable systems. It defines a small set of interfaces, derived from SOLID principles, that give explicit names and contracts to the transitions between data: the conversions, transformations, and paths that most systems leave anonymous. The interfaces compile. The semantics are defined. It is not a framework, and it is not opinionated about your architecture.

### Why SOLID

Each primitive takes the SOLID principles seriously, not as guidelines but as load-bearing constraints.

- **S, Single Responsibility.** Every primitive has exactly one concern. Converter turns one type into another; Chain chooses from multiple similar converters. Transform takes an object and turns it into another of the same type; Pipeline orders a group of them deterministically. None bleed into each other.
- **O, Open/Closed.** Install a new converter and nothing existing changes. Chain, Graph, and Domain are all open for extension by design, closed to modification by contract.
- **L, Liskov Substitution.** Converters of matching types can be freely swapped with each other, as can Transforms. The Composite rule extends this to object groups: swap one for many to extend (a Converter becomes a Chain transparently), or the reverse to narrow them for testing, staging, or stubbing (a Chain becomes a single Converter).
- **I, Interface Segregation.** The interfaces are as thin as possible. `resolve()` is one method. `apply()` is one method. You implement only what you need, and no more.
- **D, Dependency Inversion.** Chain and Pipeline are the most common injection mechanisms and you get them for free. They both speak `Converter` and `Transform`, so any conforming implementation drops in without the caller changing. Dependency injection is structural, not configured.

## The Problem

Object-oriented design gives you names and structure, but usually locks composition in at design time. Pure functional design gives you fluid composition, but usually at the cost of types, names, and the sense of where one thing ends and another begins. SolidFI aims to bridge that gap by landing at the sweet spot in the middle, where you have the best parts of both.

## The Idea

Data is simple. What's interesting is what happens between data: the transitions, the transformations, the paths from one form to another. Most systems treat those transitions as second-class citizens: anonymous functions, implicit middleware, tangled pipelines.

SolidFI treats the data and transitions between them as the primary thing. Transitions have names and types. Not because that's what makes composition possible, but because that's what makes it *legible*. You can read a composition and understand it, trace it when it breaks, and  about it as a typed system before anything runs. When something goes wrong at runtime, you can better see where the problem is, instead of looking at big stack filled mostly of anonymous functions and asynchronous continuations.

## Composition is the Point

Small pieces that know only their own contract, are assembled in explicit order, and route by self-declaration rather than external configuration. The structure is the configuration. 

Determinism is also a first-class value. Because flexibility that makes behavior unpredictable isn't flexibility, so much as chaos with a nicer name. So, determinism is the default, non-determinism has to be earned.

Registries and discovery are real problems though, and SolidFI doesn't pretend otherwise. It aims to express them in ways that are readable, typed, and composable rather than bolted on as an afterthought.

Bring your existing code. The interfaces are thin by design.

## Names for Important Things

SolidFI uses named marker types and named primitives so that every important concept has an identity, even if it represents a fairly simply concept that could be used in-place each time. Anonymous types, functions, and compositions can make readability, debuggability, and reasoning challenging.

A name is not just a label, it is a place to attach relationships, invariants, and meaning. For example, `Parameters` exists as a named marker so that the concept of "contextual data flowing into a conversion" has a home in the spec, even before its internal structure is defined.

If something matters, it has a name.

## Incremental Adoption

SolidFI is designed for incremental adoption, instead of a big-bang rewrite. You can get started quickly by wrapping existing code inside of adapters that conform to SolidFI's core abstractions: Converter and Transform, often with zero modifications to any of the code you are working with. Since interfaces are very thin, there's not much to write. 

As you progress, each type added to the system can be reused by core implementation patterns (Chain and Pipeline) for free, which can sequence complex behaviors without adding new imperative code. Later on, you can replace the adapters and have your implementation conform to the interfaces directly, and delete unnecessary code. You'll also find that a lot of the boring glue code is obselete and can be removed.

The clearest systems still have some imperative code, and SolidFI doesn't replace that. What it replaces is the glue: the hand-written dispatch tables, the anonymous middleware chains, and the code-generated adapters.

You can also use SolidFI top-down to solve problem by decomposing a problem into typed pieces from the start, creating stub implementations early, and then replacing the hard-code temporary pieces with real code as you go.

Even better you don't have to pick between the two. Add new capabilities top-down and refactor things botom-up in the same code base. 

Either way, each piece you name unlocks more and more power by pieces that now compose naturally, just without the combinatoric explosion of imperative code.

## Levels

SolidFI is organized into three levels.

**L1 is the primary surface.** Users write primarily against L1. It defines the complete set of typed, named primitives for building compositions: how data flows, how it is converted, how change is represented, and how paths through a graph are found and executed.

**L0 is the theoretical substrate.** L0 names the foundational concepts that inform L1, the mathematical and structural ideas that underlie the design. Implementers may find L0 useful as a reference layer; users generally don't need to. L0 and L1 are independent: L1 does not necessarily depend on L0.

**L2 is domain patterns.** L2 has higher-level concepts the recur in the real world (Proxy, Protocol, Transport, etc.) so often that they deserve a name and implementation that can be reused. These are built on the L1 primitives.

## Implementation Notes

While this spec is implemented in C++, it is only done so as a specification language to validate the concepts formally and produce the documentation. 

Other langagues that are simlarly strongly-typed should be able to implement most of the spec. TypeScript in particular is a natural home for it because of it's functional heritage combined with a rich typing system. C++ works because it's multi-paradigm and has all pieces necessary.

The spec and examples are currently written in an object-oriented style: classes, inheritance, explicit interfaces. This is intentional as a middle ground that it is readable in both C++ and TypeScript, and maps directly to working implementations in either language.

Most ideas map cleanly to C++ concepts and TypeScript's structural typing, where the inheritance hierarchy is replaced by `requires` constraints and duck typing, respectively. Both are valid implementations of the same spec, so the names, contracts, and composition rules stay identical. What changes is the mechanism: `implements Converter<T,U>` becomes `requires Converter<T,U>`, and the inheritance dependency disappears. In other words, this spec is written in OO to be concrete and should should be read as defining contracts, not mandating inheritance.

---

## Status

This is a personal spec, actively being developed. Reference implementation coming.

Not to be confused with Java's `@FunctionalInterface` — that's one interface with one method. This is a system of interfaces designed around composition. The plural is intentional.

## Author

Scott Tringali  
Copyright (c) 2026. All rights reserved.  
A permissive license will be applied upon publication.
