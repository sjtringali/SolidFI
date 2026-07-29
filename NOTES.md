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
| `Delegate<T>`    | `target() -> T&`           | Object indirection. L2: `Proxy`                                                    |
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
| `Router<T,U,P>`  | `Converter<T,U,P>`                 | Find-and-execute; composes Solver with Path traversal. Carries its own `failed: U` value — no Path exists to defer to when Solver finds nothing |
| `Traversal<U,P>` | `Converter<Domain,U,P>`            | Reserved. Abstract base for traversal algorithms over a Domain.                 |
| `Registry<T>`    | —                                  | Runtime complement to Extensible. Shape TBD.                                    |

---

## Open Questions

### Pathfinder — L0 or L1?

Pathfinder (untyped, Graph-bound, any T→U at runtime) may belong at L0 as the algorithm
substrate, with `Solver<T,U,P>` as the L1 typed projection — mirroring how other L0
concepts get concretized at L1. Currently treated as L1 until implementation clarifies
the right level.

---

## Design Notes

### Simple things have simple syntax

Let the language's primitives do the work. If a class has no constructor args, pass the
constructor directly -- `new Delayed(Trim)`, not `new Delayed(() => new Trim())`. A
constructor reference is already the right shape; don't wrap it in a closure just because
a factory function is the reflex.

This works for objects; primitives don't have constructors you can pass this way. That's
fine -- objects first. Primitive support is a bonus if it can be done without ugly
gymnastics, not a requirement.

Common-case friction should be zero. The complex form (closure, args) exists for when
you need it. You shouldn't pay for it when you don't.

### Names carry meaning

Named classes over anonymous objects and lambdas. The class name is documentation that
survives refactoring and shows up in stack traces, logs, and tooling. An anonymous
function has no name. When the name says everything, extra syntax is noise.

### Optional means optional

Guards (`accepts?`, `rejects?`, `handles?`) are optional properties on the interface. If
you don't need one, you simply don't implement it -- no stub, no `return true`, no
required override. The interface does not punish the common case to cover the rare one.

### Separate what is actually separate

Distinct concepts get distinct names even when they share a pattern. `Chain` vs
`Pipeline`, `Deferred` vs `Delayed` -- same structural idea, different types, different
names. Resist collapsing them into a hierarchy just because they look alike.

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
