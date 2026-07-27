@page inspiration Inspiration

SolidFI's design draws from two specific primary sources.

**Category theory.** For an accessible, code-first introduction, see Bartosz Milewski's
[*Category Theory for Programmers*](https://bartoszmilewski.com/2014/10/28/category-theory-for-programmers-the-preface/).
The key abstraction (Converter), and the rest of the structural primitives, are explained 
in the first few chapters alone. Free online, no math background assumed. The entire system 
builds on these concepts literally.

**SOLID.** The five principles the name borrows from (Single Responsibility, Open/Closed,
Liskov Substitution, Interface Segregation, and Dependency Inversion) were assembled by
Robert C. Martin from earlier object-oriented design work; the acronym itself is Michael
Feathers'. The canonical treatment is Martin's
[*Clean Architecture*](https://www.oreilly.com/library/view/clean-architecture-a/9780134494272/)
(2017), which restates and extends the principles from his earlier paper
[*Design Principles and Design Patterns*](https://staticfiles.dzone.com/end/sec/design_principles.pdf)
(2000). SolidFI aims to bring these principles to *your* code with built-in abstractions and 
patterns that gives you some, or all of these, by default.

Additionally:

**TypeScript.** The syntax and expressiveness of TypeScript provided the "look and feel" of the 
user-facing syntax in its ideal state, and, also served as the initial prototyping and 
implementation language. Only TypeScript has the all the language features (structural typing, 
declaration merging, optional parameters, powerful generics, and a few others) that make 
"P-threading" (Open Parametric Dispatch) feel natural, as if it was just meant to happen.

**Design Patterns.** The canonical reference is Gamma, Helm, Johnson, and Vlissides'
[*Design Patterns: Elements of Reusable Object-Oriented Software*](https://www.oreilly.com/library/view/design-patterns-elements/0201633612/)
(1994) — the "Gang of Four" book that named and catalogued these. It's almost impossible these days to not see the patterns
that have withstood the test of time. In particular, Chain of Responsibility (Pipeline, Chain) combined 
with Composite (Chain <-> Converter) and Strategy (Transform, Converter) form the substituability 
backbone that creates nearly all the power of decoupled compositions and parameterized dispatch. While 
they don't surface directly in the SolidFI vocabulary, understanding them is key to unlocking that power. 
The patterns turn the above ideas into reality with the languages we have, instead of an obscure academic
experiment.

