# SolidFI — Working Notes

Primitive summary. Not authoritative — Doxygen is the reference. Use these as a quick orientation or scratch space while the spec is actively being developed.

---

## L0 — Substrate

Foundational concepts that inform L1. For implementers. L0 and L1 are independent.

| Concept          | Shape                      | Notes                                                                              |
| ------------------| ----------------------------| -----------------------------------------------------------------------------------|
| `Operation<T>`   | `execute()`                | Unit of executable work over T                                                     |
| `Predicate<T>`   | `decide(T) -> bool`        | Single binary decision                                                             |
| `Filter<T>`      | `accepts(T)`, `rejects(T)` | Accept/reject pair                                                                 |
| `Composite<T,U>` | `dispatch(T) -> U`         | Ordered composition substrate                                                      |
| `Strategy<T,U>`  | name + priority            | Named, prioritized entry in a Composite                                            |
| `Delegate<T>`    | `target() -> T&`           | Removed, graduated to a real L1 concept, `Delegate<T,U,P>`. L2: `Proxy`            |
| `Goto<T,U>`      | —                          | Reserved. Directed relationship T→U; purpose TBD                                  |
| `Closed<T>`      | `get() -> T`               | Captures a T; produces it regardless of input. L1: `Literal<T>`                   |
| `Sentinel<T>`    | —                          | Reserved. Not implemented — removed along with `Failed<T>`; L1 failure is a plain per-Chain value, not a named type |
| `Optional<T>`    | —                          | May or may not hold a T                                                            |
| `Shared<T>`      | `get() -> T&`              | Shared ownership of a T                                                            |
| `Readonly<T>`    | —                          | A T that cannot be modified after construction                                     |
| `Parameters`     | —                          | Marker for user-defined contextual data                                            |
| `Graph`          | —                          | Directed graph: typed nodes (types) and typed edges (converters). L1: `Domain`     |
| `Category`       | —                          | `Graph` alias; category theory's name for the same structure. Theoretical grounding |
| `Traversal<U>`   | `traverse(Category) -> U`  | Algorithm over a Category. L1: `Traversal<U,P>` (reserved); `Path<T,U,P>` (proposed) |
| `Reduce<T>`      | `reduce([T]) -> T`         | Fold: collection -> single value                                                   |
| `Expand<T>`      | `expand(T) -> [T]`         | Unfold: single value -> collection                                                 |

## L1 — Primitives

### Core

| Concept            | Shape               | Notes                                                                  |
| --------------------| ---------------------| -----------------------------------------------------------------------|
| `Transform<T,P>`   | `apply(T,P) -> T`   | Takes T, produces T. Cannot fail; degrades to identity                 |
| `Converter<T,U,P>` | `resolve(T,P) -> U` | Takes T, produces U. Failure is a plain value of U defined by the composer (e.g. Chain), not a wrapper type. P for routing |
| `Parameters`       | empty               | Default P across all parameterized types                               |
| `Pipeline<T,P>`    | `run(T,P) -> T`     | Ordered composition of `Transform<T,P>`. IS-A `Transform<T,P>`         |
| `Chain<T,U,P>`     | `resolve(T,P) -> U` | Ordered composition of `Converter<T,U,P>`. IS-A `Converter<T,U,P>`. Carries its own `failed: U` value; install() has a 4-arg overload for a per-link failed value, else the chain's own applies |

### Extras

| Concept             | Shape                                  | Notes                                                     |
| ---------------------| ----------------------------------------| -----------------------------------------------------------|
| `Generator<T,P>`    | `Converter<Void,T,P>`                  | Produces T from nothing                                   |
| `Inverter<T,U>`     | `Converter<T,U>` + `Converter<U,T>`    | `forward()`/`reverse()`; implements both interfaces, swap in either|
| `Provider<T,U>`     | `Converter<T,U>`                       | One-way lookup; I/O oriented                              |
| `Literal<T,InputT>` | `Transform<T>` + `Converter<InputT,T>` | Captures a T; satisfies both hierarchies. L0: `Closed<T>` |
| `Delegate<T,U,P>`   | `Converter<T,U,P>`                     | Forwards to a target converter, bound eagerly or built lazily via a factory function. Not yet lazy through accepts()/rejects()/handles(); see @proposed note in the header |

### State

| Concept          | Shape                       | Notes                                                     |
| ------------------| -----------------------------| -----------------------------------------------------------|
| `Delta<T>`       | marker                      | The change between two states of T; named and first-class |
| `Differencer<T>` | `Converter<T, Delta<T>, T>` | Current + previous state → delta                          |
| `Applicator<T>`  | `Converter<T, T, Delta<T>>` | Current state + delta → next state                        |

### Compositions

| Concept          | Shape                              | Notes                                                                           |
| ------------------| ------------------------------------| --------------------------------------------------------------------------------|
| `Path<T,U,P>`    | `traverse(T,P) -> U`               | Explicitly-wired T→...→U. IS-A `Converter<T,U,P>`. Builder API provisional.    |
| `Domain`         | `install<T,U>` / `remove`          | Unordered registry of Converter edges. Holds; does not act. L0: `Graph`. L2: `Runtime` |
| `Solver<T,U,P>`  | `Converter<Domain,Path<T,U,P>,P>`  | Typed discovery; T,U fixed at compile time; composable via Chain. Carries its own `failed: Path<T,U,P>` value |
| `Pathfinder`     | `find<T,U>(T,P) -> Path`           | Untyped; Domain-bound at construction; one instance, any T→U query at runtime. |
| `Router<T,U,P>`  | `Converter<T,U,P>`                 | Find-and-execute; composes Solver with Path traversal. Carries its own `failed: U` value: no Path exists to defer to when Solver finds nothing |
| `Traversal<U,P>` | `Converter<Domain,U,P>`            | Reserved. Abstract base for traversal algorithms over a Domain.                 |
| `Registry<T>`    | —                                  | Runtime complement to Extensible. Shape TBD.                                    |

---

## Motivation: Delegate and construction order

Lazy construction is common, but when a constructor has side effects, building an object
lazily means those side effects fire at whatever point some caller happens to touch it
first. In a system with many participants that can each trigger each other indirectly,
that turns construction order into a hidden variable of runtime behavior: the same
program, run with different call patterns, can construct things in a different order and
surface different bugs depending on what the user happened to do, and when.

Imperative construction at the call site (build everything eagerly, in the order it's
written) avoids that side-effect-timing problem, but it buys nothing about dispatch
order: the order things are constructed in code has no necessary relationship to the
order they're actually reached at runtime. You can create N converters and assign their
priorities in a completely different order than you constructed them in.

Delegate's lazy constructor doesn't solve this by making construction eager again. It
solves it by tying the first-construction moment to something already structured:
Chain's priority order. Within a given dispatch, Chain probes entries in priority order,
so if several entries hold lazily-built Delegates, they get built, at most once each, in
that same relative order for that dispatch, regardless of the order their constructors
were called at the assembly site. The result is deterministic relative to the other
entries in the chain, rather than depending on incidental usage.

This doesn't make construction order fully deterministic across a program's whole
lifetime, since which entries get touched at all still depends on which inputs arrive.
The claim is narrower: entries that do get touched in the same dispatch are built in
chain-priority order, not in whatever order real usage happens to produce.

There's a real case where construction order genuinely needs to be independent of any
such structure, plugins loaded or registered in arbitrary order with no defined
relationship to each other. That's a different problem from what Delegate solves here,
and it's being handled separately later.

### Why Converter never knows about "next"

Classic Chain of Responsibility, and its JS/TS middleware descendants (Express and
Koa's `next()`, Redux's `next(action)`), gives each handler a reference to whatever
comes after it, so every handler has two jobs: do its own work, and know how to hand
off. That couples a participant to its position in a sequence it shouldn't need to know
exists, an SRP violation baked into the pattern itself. Converter has no `next`, no
reference to siblings. Chain alone owns sequencing. That's also why Delegate's
construction-order property holds for free: no participant needs to cooperate or know
about anything else for Chain's order to apply to it.

### Why Delegate fuses proxy and deferred creation

Delegate looks like it conflates two GoF patterns, Proxy (forward to a target) and
Factory (build on demand), but this is GoF's own Virtual Proxy variant: a proxy whose
target is created on first access. Splitting these into two types was considered and set
aside: one type with two constructors (bind an existing target, or bind a factory) gives
good call-site ergonomics, swap eager for lazy without changing the entry's type, with
nothing real lost by staying combined. Revisit once Generator exists and the lazy half
can be properly typed as `Generator<Converter<T,U,P>>` instead of a bare `std::function`.

### Rejected or parked, and why

- **Static `accepts()` for true zero-construction laziness.** Would let a caller ask
  "would this succeed" without building the target, but only works if Delegate knows the
  concrete target type at compile time, not just T,U,P, a bigger shape change. Parked;
  "lazy until first probed" is good enough for now.
- **Predicates stored as data** (`std::function<bool(T)>` fields on Delegate, independent
  of the target) instead of forwarding to the built object. Would get the same
  zero-construction benefit without needing the concrete type, but is still a bigger
  change than currently justified. Same status: parked.
- **Splitting Delegate into separate Proxy and Factory types.** Rejected, see above:
  GoF's Virtual Proxy already fuses them, and splitting loses call-site flexibility for
  no real gain.
- **Bare `Converter`, or a single-letter alias** (`using C = Converter<T,U,P>`) to
  shorten repeated `Converter<T,U,P>` in Delegate.hpp. Bare `Converter` doesn't compile
  there: no C++ mechanism infers T,U,P from an enclosing class for an unrelated
  template, that only works for a class referring to itself. A single-letter alias
  compiles, but reads worse, and this file doesn't have enough repetition to earn one
  either way. Kept fully spelled out, consistent with every other file. A future file
  with heavy repetition should use a descriptive alias (`ConcreteConverter`), not a
  single letter.

### Prior art

The construction-ordering half of this problem is well trodden: Mark Seemann's
Composition Root pattern (assemble the graph once, explicitly, to avoid the Service
Locator/Ambient Context anti-pattern's "temporal coupling"), Guice's eager singletons and
Dagger's compile-time graph validation (both exist to fail fast on bad wiring instead of
at some unpredictable runtime moment), Erlang/OTP supervision trees (children start in
the exact order given in the child spec list), and C++'s own static initialization order
fiasco (mitigated by the construct-on-first-use idiom). None of them unify construction
order with dispatch order the way Delegate does: DI containers resolve one
implementation per interface, not per-value dispatch among alternatives, so their
ordering machinery is necessarily separate from routing. Delegate gets its ordering for
free specifically because Chain, via the Composite rule, already had to have that same
ordering for routing.

### Example: install() order vs. dispatch order

```typescript
// Without Delegate: constructed in code order (Cache, Database, Cdn), unrelated to
// priority. If DatabaseQuery's constructor opens a connection, it opens second in real
// time even though priority 0 means it's the first thing ever dispatched to.
chain.install(2, 'cache', new CacheLookup());
chain.install(0, 'database', new DatabaseQuery());
chain.install(1, 'cdn', new CdnFetch());

// With Delegate: install() stays eager (still needs a live Converter right now), but
// each entry is now a cheap wrapper. The expensive target builds on first dispatch, in
// Chain's priority order, not install() call order.
chain.install(2, 'cache', new Delegate(() => new CacheLookup()));
chain.install(0, 'database', new Delegate(() => new DatabaseQuery()));
chain.install(1, 'cdn', new Delegate(() => new CdnFetch()));
```

---

## Open Questions

### Pathfinder — L0 or L1?

Pathfinder (untyped, Graph-bound, any T→U at runtime) may belong at L0 as the algorithm
substrate, with `Solver<T,U,P>` as the L1 typed projection — mirroring how other L0
concepts get concretized at L1. Currently treated as L1 until implementation clarifies
the right level.

---

## L2 — Domain Patterns

Stubs. Built on L1 primitives. Not yet fully specified.

| Concept         | Notes                                                          |
| -----------------| ----------------------------------------------------------------|
| `Protocol`      | Encoding and framing contract for a communication exchange     |
| `Transport`     | Physical or logical transmission channel                       |
| `Serializer<T,U>` | `Converter<T,U>` alias; named boundary marker, in-memory to wire form. One-way |
| `Codec<T,U>`    | `Inverter<T,U>`; encode/decode pair. Satisfies `Serializer<T,U>` for free      |
| `Proxy`         | Optional interception on a Converter; opt-in, default-off      |
| `Roundtrip<T,U>`| `Inverter<T,U>`; request/response pair at the protocol level   |
| `Runtime`       | `Domain` (L1) alias; a Domain realized for a specific deployment context |
| `Loader`        | `Converter<Runtime, Runtime>` — enriches a Runtime with additional converters |
