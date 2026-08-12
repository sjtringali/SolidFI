# SolidFI Guide

This guide walks through SolidFI from first principles using a running example. The code is illustrative TypeScript, close to real, but not a working program.

## Contents

1. [Getting Started](#getting-started)
2. [A Different Approach](#a-different-approach)
3. [Decomposing Into Types](#decomposing-into-types)
4. [Handling Multiple Choices](#handling-multiple-choices)
5. [Handling Failure](#handling-failure)
6. [Static Composition with Path](#static-composition-with-path)
7. [Parameters](#parameters)
8. [Transforms](#transforms)
9. [Pipeline](#pipeline)
10. [From Static Composition to Dynamic](#from-static-composition-to-dynamic)
11. [Assemblers vs Runners](#assemblers-vs-runners)
12. [Extending the System at Runtime](#extending-the-system-at-runtime)

---

## Getting Started

Let's decompose a problem top-down, to build something new from scratch. We want to build an online reader for OpenDocument files. So what if it was magic? The dream would be something like:

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

Let's add a little more intention and use a TypeScript implementation of SolidFI. First thing we do is *name* what we did with `Converter`. Converter requires one method called `resolve`, otherwise it won't compile.

```typescript
  type HTML = string;
  type Filename = string;

  class TempOpenDocumentReader implements Converter<Filename, HTML> {
    resolve(filename: Filename): HTML { /* same */ }
  }
```

That may not look like much difference, but by implementing an interface, we will gain a lot of capabilities for free later. Stay tuned.

Not bad. Here we might be tempted to open the file right inside `resolve`. Except, here's where we take a detour and start to think in types. A filename is just a string. You get bytes out of it. Then bytes turn into the HTML. The obvious solution splits it in two, and composes them externally, so they don't know about each other. Standard structured programming.

```typescript
  class FileReader implements Converter<Filename, Blob> {
    resolve(filename: Filename): Blob {
      return fs.readFileSync(filename);
    }    
  }

  class TempOpenDocumentReader implements Converter<Blob, HTML> {
    resolve(blob: Blob): HTML {
      // TODO: parse the bits from Blob something and then:
      return "<html>I am an ODF document that is " + blob.size() + " bytes long</html>";
    }    
  }

  const blob = new FileReader().resolve('document.odf');
  const html = new TempOpenDocumentReader().resolve(blob);
```

## Decomposing Into Types

So we have it broken in two, but we don't want to stuff everything into just those two, so we now iterate.

Let's say we write a container parser (it's a ZIP), then an XML parser (to find the document inside), some intermediate representation (call it a Page), and then HTML. So you write a few things that go through each intermediate representation.

```typescript
  class FileReader implements Converter<Filename, Blob>;
  class Unzipper implements Converter<Blob, XML>;
  class Parser implements Converter<XML, Page>;
  class Renderer implements Converter<Page, HTML>;
```

Now each one can be tested independently, because they are structured from the beginning as separate concerns. And because they all follow the same interface, they can be handed off to other code that's unaware of what lies beneath.

## Handling Multiple Choices

As we develop `Parser`, a wrinkle appears: the page inside the document might be text or an embedded image. The problem is `Parser` would then need to handle two things, and they should be separated, for the same reasons the rest of the converters are separate:

```typescript
  class TextParser implements Converter<XML, Page> {
    resolve(xml: XML): Page | null {
      if (isImagePage(xml)) return null;
      return parseText(xml);
    }
  }

  class ImageParser implements Converter<XML, Page> {
    resolve(xml: XML): Page | null {
      if (!isImagePage(xml)) return null;
      return parseImage(xml);
    }
  }
```

The obvious first move is a delegate, a `PageParser` that holds both and tries them in order:

```typescript
  class PageParser implements Converter<XML, Page> {
    resolve(xml: XML): Page | null {
      if (isImagePage(xml)) 
        return new ImageParser().resolve(xml);
      return new TextParser().resolve(xml);
    }
  }
```

This does work. And notice: `PageParser` IS-A `Converter<XML, Page>`, so it slots in anywhere `Parser` was. That's why we implemented the `Converter` interface, way back when, even though it looked like nothing. To any caller, `PageParser` is indistinguishable from `TextParser` or `ImageParser`. You can always swap one for the other.

But `PageParser` is still mechanical. It's just a hardcoded dispatch: the kind of code that grows a new `if` every time a new page type appears. Each converter already knows what it can handle. `PageParser` adds nothing except repetition.

`Chain` (from "Chain of Responsibility") removes this repetition. It's an ordered composition of converters, each declaring what it handles, tried in priority order until one succeeds:

```typescript
  const xmlToPage = new Chain<XML, Page>();
  xmlToPage.install(1, 'text',  new TextParser());
  xmlToPage.install(2, 'image', new ImageParser());
```

`Chain` IS-A `Converter<XML, Page>`, so it satisfies the same rule: anywhere you'd put `PageParser`, you can put `xmlToPage`. Chain is indistinguishable from a single converter to any caller. Adding a new page type is now just `xmlToPage.install(3, 'table', new TableParser())`: no delegate to update.

## Handling Failure

Converters, since they are mapping between types, can fail. It might be there's no 
representation between the source and destination (Xml and Page), or there might be 
some sort of failure performing the action that has nothing to do with T (network 
timeout, file that disappeared, service that's down). 

Chain doesn't particularly care, it just needs to know when a `resolve()` didn't 
succeed, so it can try the next converter. In the above example, we did this by 
using `Page | null` which is kinda standard, but still a bit awkward. It mixes 
up the desired result, getting the page, with how we get there.

What we need is a way to represent the failure, without committing to a single 
solution for everything.

Chain holds the failure policy in a `Failed<U>`, a small wrapper with a single 
`value` member. The reason it isn't built in is flexibility across large sets of 
disparately developed types that already exist. Forcing a single representation 
would mean either restricting which types can participate in a Chain, or wrapping 
every return value in a container you didn't choose.

For object types, `null` is the obvious default, and Chain uses it automatically 
when you don't pass a `Failed<U>`. For non-nullable types (numbers, symbols, your 
own sentinel objects), you pass one explicitly:

```typescript
  const xmlToPage = new Chain<XML, Page>();             // null by default
  const counter   = new Chain<XML, number>({ value: -1 });  // explicit sentinel
```

Per-link overrides are plain `U` values, passed directly to `install()`. Chain 
compares a link's result against the link's own sentinel (if given), or against 
the chain's `Failed<U>` otherwise. If every link fails, Chain returns the chain's 
own `Failed.value`:

```typescript
  xmlToPage.install(1, 'text',  new TextParser());
  xmlToPage.install(2, 'image', new ImageParser(), ImageParser.ERROR);
```

Chain sees `ImageParser.ERROR` come back from `ImageParser`, recognizes it as that 
link's failure, and moves on. If both links fail, Chain returns `null`, its own 
failure value.

`null`, `undefined`, a `Symbol`, a blank object, `-1`: any value works as a 
sentinel. SolidFI just needs to be able to tell success from failure, and you 
define what failure looks like. The `Failed<U>` wrapper also means any U-producing 
concept that exposes a `value` (a Provider, a Literal, a Generator) can serve 
as a Chain's failure policy directly, with no adapter needed.

Each link, by the way, is a type `Strategy` that you can define constants of, pass
around, compute, even store and load.

## Static Composition with Path

We now have our converter types: `FileReader`, `Unzipper`, the `xmlToPage` Chain, and `Renderer`, but they still need glue code to connect them. The obvious next step is to wire them by hand:

```typescript
  const blob  = new FileReader().resolve(filename);
  const xml   = new Unzipper().resolve(blob);
  const page  = xmlToPage.resolve(xml);
  const html  = new Renderer().resolve(page);
```

That works, but it's mechanical, just threading output to input, over and over. The types already encode exactly what connects to what. SolidFI has a better answer: `Path`. You declare the same sequence as a construction, and the result IS-A `Converter<Filename, HTML>`. The path exists because you made it.

```typescript
  const path = new Path<Filename, Blob>()
    .append(new FileReader())     // Filename -> Blob
    .append(new Unzipper())       // Blob -> XML
    .append(xmlToPage)            // XML -> Page  (Chain -- any step can be a Chain)
    .append(new Renderer());      // Page -> HTML

  const html = path.traverse('document.odt', {});
```

All the interesting work is in construction. `append()` advances the type at each step, and the external shape is always `Converter<Start, End>`. Because `Chain` IS-A `Converter`, it slots into any step directly. Each call to `append()` returns a new, independently usable path.

If the Chain is local to this one path and doesn't need a name, `Multipath` removes the ceremony with `toEither()`, which builds the Chain inline:

```typescript
  const path = new Multipath<Filename, Blob>()
    .to(new FileReader())
    .to(new Unzipper())
    .toEither(new TextParser(), new ImageParser())   // Chain, inline
    .to(new Renderer());
```

`Multipath` IS-A `Path` IS-A `Converter<Filename, HTML>` -- the same external shape. It adds nothing at runtime; the sugar is construction-time only.

## Parameters

In the real world, performance matters, and there's a problem here. What happens if document is five hundred pages? If someone wants to view just page 34, parsing it all just to throw it away is a lot of waste. We really want a smaller Page as a result, rather than a giant one. So let's pass that intent into the conversion:

```typescript
  const html = path.traverse('document.odt', { page: 34 });
```

Woe be unto you, if you were still using manual composition. Adding this new parameter object to every single step, threading it through a bunch of function calls hither and yon. We've all done it. It's tedious.

But since we used `Path` instead... where do we add it?

It's already there. `Converter<T,U,P>` has always had a third slot, it just defaulted to empty `Parameters`. The whole Path has been carrying P all along, passing it to every converter in sequence. Most of them don't care and ignore it. The one that does, say, `TextParser`, reads it in `resolve()` and only processes the requested page:

```typescript
  type ViewParams = { page?: number };

  class TextParser implements Converter<XML, Page, ViewParams> {
    resolve(xml: XML, params: ViewParams): Page {
      return params.page !== undefined
        ? parseOnePage(xml, params.page)
        : parseAll(xml);
    }
  }
```

The best part? `FileReader`, `Unzipper`, and `Renderer` don't change. They could declare `ViewParams` as their P and simply ignore it in `resolve()`, but they don't have to. The compiler enforces that P is consistent across the path, you can't accidentally pass the wrong type. The intent flows through each conversion end-to-end, with each converter taking what it needs and ignoring the rest.

Chain carries P the same way. Declare `Chain<XML, Page, ViewParams>` and P flows into each converter it holds:

```typescript
  const xmlToPage = new Chain<XML, Page, ViewParams>();
  xmlToPage.install(1, 'text',  new TextParser());
  xmlToPage.install(2, 'image', new ImageParser());
```

Nothing else changes. The Chain is still a `Converter<XML, Page, ViewParams>` and slots into the path exactly as before.

## Transforms

Now let's add a new feature for accessiblity. Say we want the reader to support color blindness correction, specifically, adjusting the rendered HTML so the colors work for users with deuteranopia. 

This is a Transform. Unlike Converters that change type and thus the value, Transforms, change the value, but not the type. Therefore this filter is a `Transform<HTML>`.

One really nice property of Transforms is they cannot fail. If a transform can't do anything useful with its input, it returns the original value unchanged. No failure path, no sentinel value to define, nothing you need to reserve to mean "this didn't work." The degradation floor is identity. That constraint is what makes them composable much easier than Converter. 

Still, it only should run when needed. Since P flows through the whole path, the transform just checks it directly in `apply()` and returns the original value unchanged if it doesn't apply:

```typescript
  type ViewParams = { page?: number; colorBlind?: 'deuteranopia' | 'protanopia' };

  class DeuteranopiaFilter implements Transform<HTML, ViewParams> {
    apply(html: HTML, params: ViewParams): HTML {
      return params.colorBlind === 'deuteranopia' ? adjustColorsForDeuteranopia(html) : html;
    }
  }
```

Since we are using Path to wire everything together, where does it go? With `Multipath`, it slots in with `through()`:

```typescript
  const path = new Multipath<Filename, Blob>()
    .to(new FileReader())
    .to(new Unzipper())
    .toEither(new TextParser(), new ImageParser())
    .to(new Renderer())
    .through(new DeuteranopiaFilter());   // HTML -> HTML
```

Hooray for interfaces! The caller still only holds a `Converter<Filename, HTML>`, so the transform is invisible behind the interface.

## Composing Transforms with Pipeline

What if we need more than one transform? We want to also increase font size for visual acuity. That's another `Transform<HTML, ViewParams>`, obviously independent of colorblindness correction.

Unlike Converters, transforms can never fail, so we run *all* of them: the ones that don't apply just return the original value unchanged.

`Pipeline` is the named composition for transforms, playing the same role `Chain` plays for converters. It's an ordered composition of transforms that is itself a `Transform<T,P>`:

```typescript
  type ViewParams = { page?: number; colorBlind?: 'deuteranopia' | 'protanopia'; largeText?: boolean };

  class FontSizeTransform implements Transform<HTML, ViewParams> {
    apply(html: HTML, params: ViewParams): HTML {
      return params.largeText ? increaseFontSize(html) : html;
    }
  }

  const accessibility = new Pipeline<HTML, ViewParams>();
  accessibility.install(1, 'colorblind', new DeuteranopiaFilter());
  accessibility.install(2, 'font-size',  new FontSizeTransform());
```

`Pipeline` IS-A `Transform<HTML, ViewParams>`, so it slots into a Multipath with `through()`, the same call that added a single transform:

```typescript
  const path = new Multipath<Filename, Blob>()
    ...
    .to(new Renderer())
    .through(accessibility);
```

The path doesn't know or care that `accessibility` is a Pipeline. The composite rule again: to any caller, a Pipeline is indistinguishable from a single transform.

If the Pipeline is local to this one path and doesn't need a name, `throughAll()` builds it inline:

```typescript
  const path = new Multipath<Filename, Blob>()
    ...
    .to(new Renderer())
    .throughAll(new DeuteranopiaFilter(), new FontSizeTransform());
```

Transforms aren't limited to the output end of a path. A `Transform<XML>` could normalize whitespace before parsing; a `Transform<Blob>` could strip a header before unzipping. They can live at any stage: `through()` works wherever the type matches.

```
================================================================
DRAFT -- Selector (proposed L1 addition)
The concept is stable; the wiring syntax is not yet finalized.
================================================================
```

## Transforming Parts of a Value

A homophone checker catches words that sound alike but mean different things -- "their", "there", "they're". A basic spell-checker can flag a word in isolation, but a homophone checker cannot: it needs the words around it to know which one is wrong. The right type for it is `Transform<Sentence>`, not `Transform<Word>` and not `Transform<Page>`. The problem defines the level.

```typescript
  interface Word     { text: string; }
  interface Sentence { words: Word[]; }
  interface Page     { title: string; sentences: Sentence[]; }

  class HomophoneChecker implements Transform<Sentence> {
    apply(sentence: Sentence): Sentence {
      return { ...sentence, words: correctHomophones(sentence.words) };
    }
  }
```

`HomophoneChecker` has no knowledge of `Page`. It only knows about sentences -- which is exactly as much as it needs.

The problem of finding sentences inside a page is entirely separate. That is what `Selector` expresses. A `Selector<Page, Sentence>` is a `Converter<Page, Sentence[]>` -- it resolves to zero or more sentences. An empty result is not failure; it just means nothing matched. The only class in the system that knows how sentences are stored inside a page is the selector itself:

```typescript
  class PageSentences implements Selector<Page, Sentence> {
    resolve(page: Page): Sentence[] {
      return page.sentences;
    }
  }
```

If `Page` restructures how it stores paragraphs or sentences, only `PageSentences` changes. `HomophoneChecker` is untouched.

To wire them into a `Pipeline<Page>`, an adapter applies the transform to each selected element. In languages with reference semantics -- JavaScript and TypeScript included -- the sentences are already references into the page, so updating them updates the page directly. No reconstruction needed.

```typescript
  class PerSentence implements Transform<Page> {
    constructor(
      private selector: Selector<Page, Sentence>,
      private transform: Transform<Sentence>
    ) {}

    apply(page: Page): Page {
      for (const sentence of this.selector.resolve(page, {})) {
        this.transform.apply(sentence, {});
      }
      return page;
    }
  }

  const pipeline = new Pipeline<Page>();
  pipeline.install(1, 'homophones', new PerSentence(new PageSentences(), new HomophoneChecker()));
```

Three classes. Three responsibilities. `HomophoneChecker` fixes text. `PageSentences` finds sentences. `PerSentence` applies one to the other. None of them bleeds into the others.

> **Note:** `PerSentence` above is a hand-written adapter. A future `Traversal` concept will formalize this pattern -- a named composition of a `Selector` with a `Transform` on its element type, itself a `Transform` on the container.

```
================================================================
END DRAFT
================================================================
```

## From Static Composition to Dynamic

Back to Path. As clear as it is, is still kind of annoying and mechanical. Aside from the Transforms, why do we have to tell which types fit together *at all*? Doesn't the type system already know which ones fit together? We should just be able to create them, toss them into a virtual salad bowl, and have the computer figure it out.

It does! Here we introduce `Runtime`, which is an *unordered* registry of converters, and `Router`, which searches the Runtime and executes the route it finds, as a single `Converter<Filename, HTML>`.

```typescript
  // Register known conversions. Order doesn't matter.
  const runtime = new Runtime();
  runtime.install(new FileReader());
  runtime.install(new Unzipper());
  runtime.install(new TextParser());
  runtime.install(new ImageParser());
  runtime.install(new Renderer());

  // Router IS-A Converter<Filename, HTML>
  const router = new Router<Filename, HTML>(runtime);
  const html = router.resolve('document.odt');
```

That's not magic, it's a graph traversal. This is dynamic composition replacing static composition. You don't write the glue code; the system found it and called it for you.

Under the hood, `Router` uses a `Solver` to find a `Path<Filename, HTML>` through the Runtime, then executes it. A Path may include Chains at intermediate steps -- branching is resolved at runtime when the input is known. The actual sequence of steps taken for a given input is a Trace; you cannot know the Trace until you run it. If you want to inspect or cache the Path before executing, `Solver` is there -- but for the common case, `Router` is the right tool.

## TODO: accepts/rejects/handles

Undecided if we want to talk about this here and how. It's an optimization and not core. But it could do something real if you have converters that build up distributed expressions. This decomposes the expressions themselves. 

## Assemblers vs Runners

That code right there can still be split into two pieces.

1. **Startup**: only the startup sequence knows the actual types of what is there.
2. **Runtime**: doesn't know any of that. It just uses the Runtime to do its work.

Bet this is starting to get familiar. Even though the code is tight, it's still just static composition again and can be expressed as type conversions. Startup is just converting nothing into a Runtime. It's a `Converter<null, Runtime>`.

```typescript
  class Startup implements Converter<null, Runtime> { ... }
  const runtime = new Startup().resolve(null);
```

We now have a Factory! You've already written the code.

### Bringing It Back Home

Here's the magic trick revealed. `Router<T,U>` IS-A `Converter<T,U>`, the same interface as that very first test object we wrote. Since they're both Converters, they can be substituted at any time.

```typescript
  function displayODF(filename: string, magic: Converter<Filename, HTML>) {
    const html: HTML = magic.resolve(filename);
    document.setHTML(html);
  }

  // Use the old stub we started with
  displayODF('document.odt', new TempOpenDocumentReader());

  // ...or use the working code.
  displayODF('document.odt', new Router<Filename, HTML>(runtime));
```

Whoever writes `displayODF` doesn't know or care that you just handed them a graph traversal in disguise, hidden behind a tiny `resolve`. You can swap a temporary thing for the real thing. But this also works in reverse, swap the real thing for a test stub, and now your *testing* and leverage all this composition we built.

## Extending the System at Runtime

Since Converter works on free types T and U, Runtimes themselves can be transformed. Earlier we converted nothing into a Runtime and called it "startup", so it's no big leap to convert one Runtime into another. Now you have the skeleton of a plugin loading system, and it has a natural home in the Factory part of the system.

```typescript
  class Loader implements Converter<Runtime, Runtime, string> { ... };
  var runtime = new Startup().resolve(null);
  runtime = new Loader().resolve(runtime, 'FancyVisualizationLibraryOfConverters');
```

Since it's just a converter, you can build multiple variants, swap them at compile or runtime, and choose between them with Chain. Maybe one loads local plugins, and another fetches them from (uh oh, let's think security) a secure server somewhere that you control.
