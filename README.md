# SolidFI

SolidFI (**SOLID Functional Interfaces**) is a design specification for building composable systems out of small, strongly-typed, pluggable pieces.

## Getting Started

Let's start with an example of using SolidFI in a top-down matter to decompose a problem. We want to build an online reader for OpenDocument files. So what if it was magic? The dream would be something like:

```
  const magic = new OpenDocumentReader();
  const html = magic.convert('document.odf');
```

Hey, that looks pretty good! Except we are all out of magic. Nothing happens yet. Lets get something working, even if it's wrong.

```
  class TempOpenDocumentReader implements OpenDocumentReader {
    convert(filename: string): string {
      // TODO: open the file, read the bits, parse something and then:
      return "<html>I am an ODF document " + string + "</html>";
    }    
  }
```

Well, thats kind of vague. A string to a string can be anything. Pass in a JPG, and what do you get?

## A Different Approach

Let's add a little more intention and use a TypeScript implementation of SolidFI. First thing we do is *name* what we did with `Converter`. Converter requires one method called `fetch`, otherwise it won't compile. 

```
  type HTML = string;
  type Filename = string;

  class TempOpenDocumentReader implements Converter<Filename, HTML> {
    fetch(filename: Filename): HTML { /* same */ }
  }
```

Not bad. Here we might be tempted to open the file right inside fetch - but here's where we a take detour and start to think in types. A filename is just a string. You get bytes out of it. Then bytes turn into the HTML. The obvious solution split it into two, and compose them externally, so they don't know about each other. Standard structured programming.

```
  class FileReader implements Converter<Filename, Blob> {
    fetch(filename: Filename): Blob {
      return fs.readFileSync(filename);
    }    
  }

  class TempOpenDocumentReader implements Converter<Blob, HTML> {
    fetch(blob: Blob): HTML {
      // TODO: parse the bits from Blob something and then:
      return "<html>I am an ODF document that is " + blob.size() + " bytes long</html>";
    }    
  }

  const blob = new FileReader().fetch('document.odf');
  const html = new TempOpenDocumentReader().fetch('blob');
```
  
Now iterate. Let's say we write a container parser (it's a ZIP), then an XML parser (to find the document inside), some intermediate representation (call it a Page), and then HTML. So you write a few things that go through each intermediate representation.

```
  class FileReader implements Converter<Filename, Blob>;
  class Unzipper implements Converter<Blob, XML>;
  class Parser implements Converter<XML, Page>;
  class Renderer implements Converter<Page, HTML>;
```

Now each write one can test independently, because they are structured from the beginning as separate concerns. And because the all follow the same interface, they can be handed off to other code that's unaware of lies beneath.

## From static composition to dynamic

But the top-level glue code is kind of annoying. It just a more verbose way of writing functions chained together, so why not just do `loadFile().then().unzip().then().parse().then().render()`? Why have objects that wrap these functions?

That's one way of looking at it. But step back a bit and ask another question: why do we have to tell which types fit together *at all*? Doesn't the type system already know which ones fit? 

It does. So let's add more magic. Here we introduce `Runtime`, which is an *unordered* list of Converters, and `Solver`, which looks at a runtime, and finds a path through them. 
```
  // Setup the list of known conversions. Uses typescript constructor syntax.
  const allConverters = new Runtime();
  allConverters.installNew(Parser, FileReader, HtmlReader, Unzipper); // Unordered!

  // Find a way to convert from Filename to HTML
  solver = new Solver<Filename, HTML>(runtime);
  const html = solver.solve('document.odt' as Filename);
```

That's not magic, it's a graph traversal. And this is dynamic composition of all these function calls, replacing static composition. You don't write the glue code; the system found it and calls it for you.

## Keep going!

That code right there can split be split into two pieces. 

1. Startup: only the startup sequence knows the acutal types of what is there.
2. Runtime: doesn't know about any of that. It calls `solver` to get the HTML.

Bet this is starting to get familiar. Even though the code is tight, it still is just static composition again and can be expressed as type conversions. Startup is just converting nothing into a Runtime. We now have a Factory.

```
  class Startup implements Converter<null, Runtime> { ... }
  const runtime = new Startup();
```

In other words: you've already written the code, just label what it does with the right types.

### Bringing it back home

Here's the magic trick revealed. Look carefully: Solver<T,U> itself *really is* is a Converter<T,U>. It's the same thing as the test object we first wrote. Since they are both Converters via the Composite pattern, they can be substituted at runtime.

```
  function displayODF(filename: string, magic: Converter<Filename, HTML>) {
    const html: HTML = magic.fetch('filename');
    document.setHTML(html);
  }

  // Use the old stub we started with
  render('document.odt', new TempOpenDocumentReader);

  // ...or use the working code.
  render('document.dot', new Solver<Filename, HTML>);
```

Whoever writes displayODF doesn't know or care that you just handed them a graph traversal in disguise, hidden behind a tiny "fetch". You can swap a temporary thing for the real thing. But this also works in reverse, you can swap the real thing for a test stub, making *testing* composoble.

## Extending the system at runtime

Since Converter and Transformer work on free types T and U, Runtimes themselves can be transformed. Earlier we converted nothing into a Runtime and called it "startup", so it's no big leap to convert one runtime into another. Now you have a plugin loading system, and it has a natural home in the Factory part of the system.

```
  class PluginLoader implements Converter<Runtime, Runtime>
```

## Parameterized Traversal, Level 1 - Choosing from multiple handlers

TODO: Just placeholder text

We already have the vocabulary to express this built into Converter. Chain<T,U>: 
1. accepts() and rejects() form the selection criteria that is stateless based on the request (can this handler ever handle this request?)
2. fetch() returns Failed<T> to indicate that it couldn't be handled at runtime (did the handler succeed handling this request?)

## Parameterized Traversal, Level 2 - Routing by request

TODO: Just placeholder text

While this works, it's missing still something: what specfically do we want to get out the document? Wouldn't it be useful to just grab page 36, so we don't have to transfer the entire thing? Back to the magic drawing board. Start with what you wish worked, and then make it so. Building off our previous example:

```
// 1. Create runtime of types
// 2. Bind solver to that runtime
const html: HTML = solver.fetch('document.odt', { page: 35 });
```

All this time, we really have been using Converter<T, U, P = void>, P just being empty. Everywhere. Each Converter, when called will be given both the input (T) plus the parameters (P). `fetch` really looks like. But in this example, not everyone cares about any given parameter. A 'page' doesn't exist inside a Blob or ZIP. So we only have to change one:

```
type P = {};
class FileReader implements Converter<Filename, Blob, Parameters>;
class Unzipper implements Converter<Blob, XML, P>;
class Parser implements Converter<XML, Page, P>;
class Renderer implements Converter<Page, HTML, P>;

class Parser implements Converter<XML, Page, P> {
  fetch(xml: XML, options: P): Page {
    return options.page ? doParse(options.page) : doParse('all');
  }
}
```

## Parameterized Traversal, Level 3 - Proxying and Alternate Routes

TODO: (1) Proxy objects can be built by simply adding to the chain before something else (non-intrusive)
TODO: (2) But converters can have their own local knowledge of a delegate (intrusive)
TODO: (3) This isn't duplicated effort: the caller always controls things in the first place, so it can always proxy things without a Converter's consent.

# END EXAMPLE

## The Problem

Object-oriented design gives you names and structure, but usually locks composition in at design time. Pure functional design gives you fluid composition, but usually the cost of types, names, and the sense of where one thing ends and another begins.

Determinism is a first-class value... because flexibility that makes behavior unpredictable isn't flexibility, so much as chaos with a nicer name.

Neither is wrong. But neither is complete.

## This Spec

While this spec is implemeted in C++, it's only done so as a specification language to validate the concepts formally and produce the documentation. A full implementation in C++ or TypeScript is possible.

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
| `Failure<U>`     | `value() -> U`             | Sentinel value for U; non-intrusive specialization. L1: `Failure<U>`     |
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
| `Converter<T,U,P>` | `fetch(T,P) -> U`    | Takes T, produces U. Failure is `Failure<U>::value()`. P for routing |
| `Chain<T,U,P>`     | `resolve(T,P) -> U`  | Ordered composition of `Converter`. Is itself a `Converter<T,U,P>`  |
| `Parameters`       | empty                | Default P across all parameterized types                           |
| `Failure<U>`       | `value() -> U`       | Sentinel for failed fetch(). Non-intrusive; specialize to opt in   |

##### Extras

| Concept             | Shape                                  | Notes                                                     |
| ---------------------| ----------------------------------------| -----------------------------------------------------------|
| `Generator<T,P>`    | `Converter<Void,T,P>`               | Produces T from nothing                                   |
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
| `Solver<T,U,P>`  | `solve(T,P) -> U`         | Graph bound at construction; finds and executes T→U path   |

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
