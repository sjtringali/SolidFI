# SolidFI Guide

This guide walks through SolidFI from first principles using a running example. The code is illustrative TypeScript — close to real, but not a working program.

## Contents

1. [Getting Started](#getting-started)
2. [A Different Approach](#a-different-approach)
3. [Static Composition with Path](#static-composition-with-path)
4. [Parameters](#parameters)
5. [Multiple Choices at One Step](#multiple-choices-at-one-step)
6. [Failed&lt;T&gt;](#failedt)
7. [Transform](#transform)
8. [Pipeline](#pipeline)
9. [From Static Composition to Dynamic](#from-static-composition-to-dynamic)
10. [Keep Going](#keep-going)
11. [Extending the System at Runtime](#extending-the-system-at-runtime)
12. [Parameterized Traversal, Level 1](#parameterized-traversal-level-1--choosing-from-multiple-handlers)
13. [Parameterized Traversal, Level 2](#parameterized-traversal-level-2--routing-by-request)
14. [Parameterized Traversal, Level 3](#parameterized-traversal-level-3--proxying-and-alternate-routes)

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

The four converters above still need glue code to connect them. The obvious next step is to wire them by hand:

```typescript
  const blob = new FileReader().fetch(filename);
  const xml  = new Unzipper().fetch(blob);
  const page = new Parser().fetch(xml);
  const html = new Renderer().fetch(page);
```

That works, but it's mechanical — just threading output to input, over and over. The types already encode exactly what connects to what. SolidFI has a better answer: `Path`. You declare the same sequence as a construction, and the result IS-A `Converter<Filename, HTML>`. The path exists because you made it.

```typescript
  const path = new Path<Filename>()
    .to(new FileReader())     // Filename -> Blob
    .to(new Unzipper())       // Blob -> XML
    .to(new Parser())         // XML -> Page
    .to(new Renderer());      // Page -> HTML

  const html = path.traverse('document.odt');
```

All the interesting work is in construction — `to()` advances the type, `through()` applies transforms that hold the type. The external shape is always `Converter<Start, End>`.

Path is the static answer: the path is known, fixed, and declared. When you don't know the path in advance, read on.

## Parameters

The document could be five hundred pages. If someone wants to view page 34, parsing all five hundred pages first is wasteful. What we really want is to pass that intent into the conversion:

```typescript
  const html = path.traverse('document.odt', { page: 34 });
```

This is already there. `Converter<T,U,P>` has always had a third slot — it just defaulted to empty `Parameters`. The whole path has been carrying P all along, passing it to every converter in sequence. Most of them don't care and ignore it. The one that does — say, `Parser` — reads it in `fetch()` and only processes the requested page:

```typescript
  type ViewParams = { page?: number };

  class Parser implements Converter<XML, Page, ViewParams> {
    fetch(xml: XML, params: ViewParams): Page {
      return params.page !== undefined
        ? parseOnePage(xml, params.page)
        : parseAll(xml);
    }
  }
```

`FileReader`, `Unzipper`, and `Renderer` don't change. They declare `ViewParams` as their P and simply ignore it in `fetch()`. The compiler enforces that P is consistent across the path — you can't accidentally drop it or pass the wrong type. The intent flows end-to-end, each converter taking what it needs and ignoring the rest.

## Multiple Choices at One Step

Now a different wrinkle. Page 34 turns out not to be text at all — it's one big embedded image. `Parser` can't handle it. We need a `TextParser` and an `ImageParser`, both producing `Page`, with the right one selected at runtime.

The obvious first move is a delegate — a `PageParser` that holds both and tries them in order:

```typescript
  class PageParser implements Converter<XML, Page, ViewParams> {
    fetch(xml: XML, params: ViewParams): Page | null {
      if (isImagePage(xml)) return new ImageParser().fetch(xml, params);
      return new TextParser().fetch(xml, params);
    }
  }

  const path = new Path<Filename>()
    .to(new FileReader())
    .to(new Unzipper())
    .to(new PageParser())     // XML -> Page: delegates internally
    .to(new Renderer());
```

This works. And notice: `PageParser` is a `Converter<XML, Page, ViewParams>` — it drops straight into the path without any changes to the surrounding code. That's the composite rule: to any caller, `PageParser` is indistinguishable from `TextParser` or `ImageParser`. You can always swap one for the other.

But `PageParser` is mechanical. It's just a hardcoded dispatch — the kind of code that grows a new `if` every time a new page type appears. The interesting logic is already in `TextParser` and `ImageParser` via `accepts()`. `PageParser` adds nothing except repetition.

`Chain` removes the repetition. It's an ordered composition of converters, each declaring what it handles, tried in priority order until one succeeds:

```typescript
  const xmlToPage = new Chain<XML, Page, ViewParams>();
  xmlToPage.install(1, 'text',  new TextParser());
  xmlToPage.install(2, 'image', new ImageParser());

  const path = new Path<Filename>()
    .to(new FileReader())
    .to(new Unzipper())
    .to(xmlToPage)            // same slot, same interface, no PageParser
    .to(new Renderer());
```

`Chain` IS-A `Converter<XML, Page, ViewParams>` — it slots in exactly where `PageParser` was. The path doesn't change. `Renderer` never knows. Adding a new page type is now just `xmlToPage.install(3, 'table', new TableParser())` — no delegate to update.

And if this choice is local to one path, even the named `xmlToPage` is more than you need. `toEither()` builds the Chain inline:

```typescript
  const path = new Path<Filename>()
    .to(new FileReader())
    .to(new Unzipper())
    .toEither(new TextParser(), new ImageParser())
    .to(new Renderer());
```

Three steps, same result. Most of the code you needed to write was already written.

## Failed&lt;T&gt;

Chain needs to know when a `fetch()` didn't succeed so it can try the next converter. SolidFI uses `Failed<T>` for this — but what `Failed<T>` actually *is* is left entirely to you.

`fetch()` is expected to be asynchronous. That means it can fail for reasons that have nothing to do with T or P — a network timeout, a file that disappeared, a service that's down. `accepts()`, `rejects()`, and `handles()` can't see any of that; they're fast, synchronous gates that reason only about the inputs. `Failed<T>` is what bridges the gap: it lets `fetch()` signal failure at runtime, whatever the cause, so Chain can respond — try the next converter, escalate, or give up cleanly.

`null` is a perfectly valid choice, as long as your type allows it. In TypeScript that might mean `Page | null`, `Page | undefined`, a `Symbol`, or an empty sentinel object. In C++ it might be `std::optional<Page>`. There are no constraints — SolidFI just needs to be able to tell success from failure, and you define what failure looks like for your type.

The reason it isn't built in is flexibility. A `Page` in one system might use `null`; in another it might be an `Optional<Page>`; in a third it might be a discriminated union. Forcing a single representation would mean either restricting which types can participate in a Chain, or wrapping every return value in a container you didn't choose. Neither is acceptable.

Here's what it looks like in practice. We declare that `null` means failure for `Page`, and `TextParser` returns it when it encounters something it can't handle:

```typescript
  // Declare what failure looks like for Page
  type Failed<Page> = null;

  class TextParser implements Converter<XML, Page, ViewParams> {
    accepts(xml: XML): boolean {
      return !isImagePage(xml);   // fast gate: don't even try if it's an image
    }

    fetch(xml: XML, params: ViewParams): Page | null {
      if (isImagePage(xml)) return null;  // shouldn't get here, but defensive
      return parseText(xml, params);
    }
  }

  class ImageParser implements Converter<XML, Page, ViewParams> {
    accepts(xml: XML): boolean {
      return isImagePage(xml);
    }

    fetch(xml: XML, params: ViewParams): Page | null {
      if (!isImagePage(xml)) return null;
      return parseImage(xml, params);
    }
  }
```

Chain sees `null` come back from `TextParser`, recognises it as `Failed<Page>`, and moves on to `ImageParser`. The Chain wiring doesn't change — you just told it what failure looks like for your type.

## Transform

Unlike converters, transforms cannot fail. A `Transform<T>` takes a T and returns a T — the same type in and out, always. If a transform can't do anything useful with its input, it returns the original value unchanged. There is no failure path, no `Failed<T>`, no sentinel. The degradation floor is identity.

That constraint is what makes them composable without ceremony. Say we want the reader to support color blindness correction — adjusting the rendered HTML so the colors work for users with deuteranopia. That's a `Transform<HTML>`: same type in and out, never fails.

But it should only run when the user asked for it. That's what `handles(P)` is for — the same P flowing through the whole path now serves double duty: routing the document request *and* declaring which transforms should apply.

```typescript
  type ViewParams = { page?: number; colorBlind?: 'deuteranopia' | 'protanopia' };

  class DeuteranopiaFilter implements Transform<HTML, ViewParams> {
    handles(params: ViewParams): boolean {
      return params.colorBlind === 'deuteranopia';
    }

    apply(html: HTML, params: ViewParams): HTML {
      return adjustColorsForDeuteranopia(html);
    }
  }
```

If `colorBlind` isn't set, `handles()` returns false and the transform is skipped — the HTML passes through unchanged. No conditional logic in the caller, no separate code path. The transform declares its own activation condition.

We add it to the path with `through()`:

```typescript
  const path = new Path<Filename>()
    .to(new FileReader())
    .to(new Unzipper())
    .toEither(new TextParser(), new ImageParser())
    .to(new Renderer())
    .through(new DeuteranopiaFilter());   // HTML -> HTML
```

The caller still holds a `Converter<Filename, HTML>`. The transform is invisible behind the interface.

## Pipeline

Now we want to also increase font size for visual acuity. That's another `Transform<HTML, ViewParams>` — independent of colorblindness correction, and for a different user need. It also declares its own activation condition:

```typescript
  type ViewParams = { page?: number; colorBlind?: 'deuteranopia' | 'protanopia'; largeText?: boolean };

  class FontSizeTransform implements Transform<HTML, ViewParams> {
    handles(params: ViewParams): boolean {
      return params.largeText === true;
    }

    apply(html: HTML, params: ViewParams): HTML {
      return increaseFontSize(html);
    }
  }
```

We can add it as a second `through()`:

```typescript
  const path = new Path<Filename>()
    ...
    .to(new Renderer())
    .through(new DeuteranopiaFilter())
    .through(new FontSizeTransform());
```

Or use `throughAll()` — the inline form that composes multiple transforms in one call:

```typescript
    .throughAll(new DeuteranopiaFilter(), new FontSizeTransform());
```

But what if these two transforms belong to an accessibility module, assembled together and handed to the path as a unit? That's `Pipeline` — an ordered composition of transforms that is itself a `Transform<T,P>`.

```typescript
  const accessibility = new Pipeline<HTML, ViewParams>();
  accessibility.install(1, 'colorblind', new DeuteranopiaFilter());
  accessibility.install(2, 'font-size',  new FontSizeTransform());

  const path = new Path<Filename>()
    ...
    .to(new Renderer())
    .through(accessibility);
```

The path doesn't know or care that `accessibility` is a Pipeline. It holds a `Transform<HTML, ViewParams>` — and Pipeline IS-A `Transform<HTML, ViewParams>`. The composite rule again: to any caller, a Pipeline is indistinguishable from a single transform.

Each transform inside the Pipeline still makes its own `handles()` decision. If the user didn't request colorblind correction, `DeuteranopiaFilter` is skipped. If they didn't request larger text, `FontSizeTransform` is skipped. If neither applies, the Pipeline degrades to identity — the HTML passes through unchanged.

A transform that rejects its input is skipped; the next one in priority order runs instead. If none run, the Pipeline returns the original value unchanged — it degrades to identity, the same floor every Transform guarantees.

Transforms aren't limited to the output end of a path. A `Transform<XML>` could normalize whitespace before parsing; a `Transform<Blob>` could strip a header before unzipping. They can live at any stage — `through()` works wherever the type matches.

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
