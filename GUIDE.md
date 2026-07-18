# SolidFI Guide

This guide walks through SolidFI from first principles using a running example. The code is illustrative TypeScript — close to real, but not a working program.

---

## Getting Started

Let's decompose a problem top-down. We want to build an online reader for OpenDocument files. So what if it was magic? The dream would be something like:

```typescript
  const magic = new OpenDocumentReader();
  const html = magic.convert('document.odf');
```

Hey, that looks pretty good! Except we are all out of magic. Nothing happens yet. Lets get something working, even if it's wrong.

```typescript
  class TempOpenDocumentReader implements OpenDocumentReader {
    convert(filename: string): string {
      // TODO: open the file, read the bits, parse something and then:
      return "<html>I am an ODF document " + string + "</html>";
    }    
  }
```

Well, that's kind of vague. A string to a string can be anything. Pass in a JPG, and what do you get?

## A Different Approach

Let's add a little more intention and use a TypeScript implementation of SolidFI. First thing we do is *name* what we did with `Converter`. Converter requires one method called `fetch`, otherwise it won't compile.

```typescript
  type HTML = string;
  type Filename = string;

  class TempOpenDocumentReader implements Converter<Filename, HTML> {
    fetch(filename: Filename): HTML { /* same */ }
  }
```

Not bad. Here we might be tempted to open the file right inside fetch — but here's where we take a detour and start to think in types. A filename is just a string. You get bytes out of it. Then bytes turn into the HTML. The obvious solution splits it into two, and composes them externally, so they don't know about each other. Standard structured programming.

```typescript
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
  const html = new TempOpenDocumentReader().fetch(blob);
```

Now iterate. Let's say we write a container parser (it's a ZIP), then an XML parser (to find the document inside), some intermediate representation (call it a Page), and then HTML. So you write a few things that go through each intermediate representation.

```typescript
  class FileReader implements Converter<Filename, Blob>;
  class Unzipper implements Converter<Blob, XML>;
  class Parser implements Converter<XML, Page>;
  class Renderer implements Converter<Page, HTML>;
```

Now each one can be tested independently, because they are structured from the beginning as separate concerns. And because they all follow the same interface, they can be handed off to other code that's unaware of what lies beneath.

## Static Composition with Path

The four converters above still need glue code to connect them. You could call each `fetch()` manually in sequence, but SolidFI has a better answer: `Path`. You wire the path explicitly at construction, and the result IS-A `Converter<Filename, HTML>`. The path exists because you made it.

```typescript
  const path = new Path<Filename>()
    .to(new FileReader())                               // Filename -> Blob
    .to(new Unzipper())                                 // Blob -> XML
    .toEither(new TextParser(), new ImageParser())      // XML -> Page: each declares what it handles
    .to(new Renderer());                                // Page -> HTML

  const html = path.traverse('document.odt');
```

All the interesting work is in construction — `to()` advances the type, `toEither()` declares a Chain at that step, `through()` applies transforms that hold the type. The external shape is always `Converter<Start, End>`.

`toEither()` with multiple converters declares a `Chain` at that step. Each converter uses `accepts()` and `rejects()` to say what it handles; the chain tries them in order until one succeeds. Here, page 34 turns out not to be text at all — it's one big embedded image. `TextParser` rejects it; `ImageParser` accepts it. Both produce a `Page`, so `Renderer` never knows the difference.

Path is the static answer: the path is known, fixed, and declared. When you don't know the path in advance, read on.

But Path and Solver aren't opposites — a Path can seed a Runtime. The path is disassembled, and each converter and transform is installed into the Graph individually. Duplicates are silently discarded. This means you can build an explicit path with Path, use it directly, and hand the same Path to the dynamic system — no rewriting, no duplication. Path can be the thing that builds the Graph that Solver traverses.

## From Static Composition to Dynamic

But the top-level glue code, as clear as it is, is still kind of annoying. Why do we have to tell which types fit together *at all*? Doesn't the type system already know which ones fit?

It does. So let's add more magic. Here we introduce `Runtime`, which is an *unordered* list of Converters, and `Solver`, which looks at a runtime and finds a path through them.

```typescript
  // Setup the list of known conversions. Order doesn't matter.
  const allConverters = new Runtime();
  allConverters.installNew(TextParser, ImageParser, FileReader, Unzipper); // Unordered!

  // Find a way to convert from Filename to HTML
  const solver = new Solver<Filename, HTML>(allConverters);
  const html = solver.solve('document.odt' as Filename);
```

That's not magic, it's a graph traversal. And this is dynamic composition of all these function calls, replacing static composition. You don't write the glue code; the system found it and calls it for you.

## Keep Going

That code right there can still be split into two pieces.

1. **Startup**: only the startup sequence knows the actual types of what is there.
2. **Runtime**: doesn't know about any of that. It calls `solver` to get the HTML.

Bet this is starting to get familiar. Even though the code is tight, it's still just static composition again and can be expressed as type conversions. Startup is just converting nothing into a Runtime. We now have a Factory.

```typescript
  class Startup implements Converter<null, Runtime> { ... }
  const runtime = new Startup().fetch(null);
```

In other words: you've already written the code, just label what it does with the right types.

### Bringing It Back Home

Here's the magic trick revealed. `Solver<T,U>` itself *really is* a `Converter<T,U>`. It's the same thing as the test object we first wrote. Since they are both Converters via the Composite pattern, they can be substituted at any time.

```typescript
  function displayODF(filename: string, magic: Converter<Filename, HTML>) {
    const html: HTML = magic.fetch(filename);
    document.setHTML(html);
  }

  // Use the old stub we started with
  displayODF('document.odt', new TempOpenDocumentReader());

  // ...or use the working code.
  displayODF('document.odt', new Solver<Filename, HTML>(runtime));
```

Whoever writes `displayODF` doesn't know or care that you just handed them a graph traversal in disguise, hidden behind a tiny `fetch`. You can swap a temporary thing for the real thing. But this also works in reverse — you can swap the real thing for a test stub, making *testing* composable.

## Extending the System at Runtime

Since Converter works on free types T and U, Runtimes themselves can be transformed. Earlier we converted nothing into a Runtime and called it "startup", so it's no big leap to convert one runtime into another. Now you have a plugin loading system, and it has a natural home in the Factory part of the system.

```typescript
  class PluginLoader implements Converter<Runtime, Runtime>
```

## Parameterized Traversal, Level 1 — Choosing from Multiple Handlers

TODO: Just placeholder text

We already have the vocabulary to express this built into Converter. Chain<T,U>:
1. `accepts()` and `rejects()` form the selection criteria based on the request — can this handler ever handle this request?
2. `fetch()` returns `Failed<T>` to indicate that it couldn't be handled at runtime — did the handler succeed?

## Parameterized Traversal, Level 2 — Routing by Request

TODO: Just placeholder text

While this works, it's missing something: what specifically do we want to get out of the document? Wouldn't it be useful to just grab page 36, so we don't have to transfer the entire thing? Back to the magic drawing board. Start with what you wish worked, and then make it so. Building off our previous example:

```typescript
  // 1. Create runtime of types
  // 2. Bind solver to that runtime
  const html: HTML = solver.solve('document.odt', { page: 35 });
```

All this time, we really have been using `Converter<T, U, P = void>`, P just being empty. Each Converter, when called, will be given both the input (T) plus the parameters (P). But in this example, not everyone cares about any given parameter. A `page` doesn't exist inside a Blob or ZIP. So we only have to change the one that cares:

```typescript
  type P = { page?: number };
  class FileReader implements Converter<Filename, Blob, Parameters>;
  class Unzipper implements Converter<Blob, XML, P>;
  class TextParser implements Converter<XML, Page, P> {
    fetch(xml: XML, options: P): Page {
      return options.page ? doParse(options.page) : doParse('all');
    }
  }
  class Renderer implements Converter<Page, HTML, P>;
```

## Parameterized Traversal, Level 3 — Proxying and Alternate Routes

TODO:
1. Proxy objects can be built by simply adding to the chain before something else (non-intrusive)
2. But converters can have their own local knowledge of a delegate (intrusive)
3. This isn't duplicated effort: the caller always controls things in the first place, so it can always proxy things without a Converter's consent.
