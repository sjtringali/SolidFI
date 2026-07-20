# SolidFI — Working Notes

Primitive summary. Not authoritative — Doxygen is the reference. Use these as a quick orientation or scratch space while the spec is actively being developed.

---

## L0 — Substrate

Foundational concepts that inform L1. For implementers. L0 and L1 are independent.

| Concept          | Shape                      | Notes                                                                    |
| ------------------| ----------------------------| --------------------------------------------------------------------------|
| `Operation<T>`   | `execute()`                | Unit of executable work over T                                           |
| `Predicate<T>`   | `decide(T) -> bool`        | Single binary decision                                                   |
| `Filter<T>`      | `accepts(T)`, `rejects(T)` | Accept/reject pair                                                       |
| `Composite<T,U>` | `dispatch(T) -> U`         | Ordered composition substrate                                            |
| `Strategy<T,U>`  | name + priority            | Named, prioritized entry in a Composite                                  |
| `Delegate<T>`    | `target() -> T&`           | Object indirection. L2: `Proxy`                                          |
| `Goto<T,U>`      | —                          | Reserved. Directed relationship T→U; purpose TBD                         |
| `Closed<T>`      | `get() -> T`               | Captures a T; produces it regardless of input. L1: `Literal<T>`          |
| `Sentinel<T>`    | —                          | Abstract signal of absence/failure. L1: `Failed<T>`                      |
| `Optional<T>`    | —                          | May or may not hold a T                                                  |
| `Shared<T>`      | `get() -> T&`              | Shared ownership of a T                                                  |
| `Readonly<T>`    | —                          | A T that cannot be modified after construction                           |
| `Parameters`     | —                          | Marker for user-defined contextual data                                  |
| `Category`       | —                          | Objects (types) + arrows (converters). L1: `Graph`. L2: `Runtime`        |
| `Traversal<U>`   | `traverse(Category) -> U`  | Algorithm over a Category. L1: `Path<T,U,P>`                             |
| `Reduce<T>`      | `reduce([T]) -> T`         | Fold: collection -> single value                                          |
| `Expand<T>`      | `expand(T) -> [T]`         | Unfold: single value -> collection                                        |

## L1 — Primitives

### Core

| Concept            | Shape                | Notes                                                                 |
| --------------------| ----------------------| ----------------------------------------------------------------------|
| `Transform<T>`     | `apply(T) -> T`      | Takes T, produces T. Cannot fail; degrades to identity                |
| `Converter<T,U,P>` | `fetch(T,P) -> U`    | Takes T, produces U. Failure is `Failed<T>`. P for routing            |
| `Parameters`       | empty                | Default P across all parameterized types                              |
| `Failed<T>`        | —                    | Explicit failure signal. Non-intrusive; type-distinct from T          |

### Extras

| Concept             | Shape                                  | Notes                                                     |
| ---------------------| ----------------------------------------| -----------------------------------------------------------|
| `Generator<T,P>`    | `Converter<Void,T,P>`                  | Produces T from nothing                                   |
| `Inverter<T,U>`     | `Converter<T,U>`                       | Guarantees bidirectional T↔U                              |
| `Provider<T,U>`     | `Converter<T,U>`                       | One-way lookup; I/O oriented                              |
| `Literal<T,InputT>` | `Transform<T>` + `Converter<InputT,T>` | Captures a T; satisfies both hierarchies. L0: `Closed<T>` |

### State

| Concept          | Shape                       | Notes                                                     |
| ------------------| -----------------------------| -----------------------------------------------------------|
| `Delta<T>`       | marker                      | The change between two states of T; named and first-class |
| `Differencer<T>` | `Converter<T, Delta<T>, T>` | Current + previous state → delta                          |
| `Applicator<T>`  | `Converter<T, T, Delta<T>>` | Current state + delta → next state                        |

### Compositions

| Concept          | Shape                     | Notes                                                                                                                                           |
| ------------------| ---------------------------| ------------------------------------------------------------------------------------------------------------------------------------------------|
| `Pipeline<T>`    | `run(T) -> T`             | Ordered composition of `Transform<T>`. Is itself a `Transform<T>`                                                                               |
| `Chain<T,U,P>`   | `resolve(T,P) -> U`       | Ordered composition of `Converter`. Is itself a `Converter<T,U,P>`                                                                              |
| `Path<T,U,P>`    | `traverse(T,P) -> U`      | Explicitly-wired T→...→U path. IS-A `Converter<T,U,P>`. `to()` adds a step; `toEither()` adds a Chain; `through()` adds a Pipeline. Can seed a `Graph`. |
| `Registry<T>`    | —                         | Runtime complement to Extensible. Shape TBD.                                                                                                    |
| `Graph`          | `install<T,U>` / `remove` | Unordered registry of Converter edges. Holds; does not act                                                                                      |
| `Solver<T,U,P>`  | `Converter<Graph,Path<T,U,P>,P>` | Typed; T,U fixed at compile time; composable. See Open Questions.            |
| `Pathfinder`     | `find<T,U>(T,P) -> Path`         | Untyped; Graph bound at construction; one instance, any T→U query. See Open Questions.  |
| `Router<T,U,P>`  | `Converter<T,U,P>`        | Find-and-execute; composes Solver with Path traversal; IS-A Converter                   |

---

## Open Questions

### Solver — typed vs untyped

Two distinct L1 concepts are needed; neither replaces the other.

**`Solver<T,U,P>`** (typed): T and U are known at compile time. Shape is
`Converter<Graph, Path<T,U,P>, P>` — takes a Graph, returns a Path. Pure discovery;
does not execute. Composable: multiple typed Solvers in a Chain, each trying a different
traversal strategy, first to find a valid path wins. Dogfoods the spec.

**DynamicSolver** (working name; candidate: `Pathfinder`): T and U are only known at
runtime. One instance serves any T→U query against its bound Graph. Cannot be expressed
as a Converter — there is no fixed T or U to put in the template. Necessary because
without it, handling a runtime-unknown T→U requires building a typed Solver per pair and
a picker to choose among them — the combinatorial expansion the spec exists to prevent.
Same argument that justifies Chain over a hand-written dispatcher.

**Router<T,U,P>** is orthogonal to both: `Converter<T, U, P>`, takes a value, returns a
result. Composes Solver or DynamicSolver with Path traversal. The "one stop shopping"
entry point for callers who just want T→U and don't care how.

**Unresolved:** final name for DynamicSolver (`Pathfinder` is the leading candidate).
Whether DynamicSolver belongs at L0 as the algorithm substrate with Solver<T,U,P> as
the L1 projection — mirroring the Sentinel→Failed pattern.

---

## L2 — Domain Patterns

Stubs. Built on L1 primitives. Not yet fully specified.

| Concept         | Notes                                                          |
| -----------------| ----------------------------------------------------------------|
| `Protocol`      | Encoding and framing contract for a communication exchange     |
| `Transport`     | Physical or logical transmission channel                       |
| `Serialization` | Converter between in-memory representation and serialized form |
| `Proxy`         | Optional interception on a Converter; opt-in, default-off      |
| `Roundtrip`     | Bidirectional communication; builds on `Inverter<T,U>`         |
| `Runtime`       | Graph with dynamic plugin-loading machinery                    |
