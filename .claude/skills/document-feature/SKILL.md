---
name: document-feature
description: Writes or updates the R-Type documentation site (Starlight pages in docs/src/content/docs/, plus the README) for a feature, in the house style of the pages already merged. Use it whenever a feature is finished, before committing it or opening its pull request, as soon as the change adds or modifies something another team uses or must respect (a class, a component, a named constant, a folder, a protocol message, a command-line option, a build step), and whenever someone asks to document, describe on the site, or write a page about part of the code, even if they never say "docs" or "Starlight".
---

# Documenting a feature

The site (<https://nique-epi.github.io/R-Type/>) is read by the four teams and by the Epitech jury. It stays worth reading only if every page sounds like the same author and states what the code really does. This skill puts a finished feature on the right pages, in the style of the pages already merged (`architecture.md`, `engine.md`, `project-layout.md`), and checks that the site still builds.

The reader to write for: a developer from another team who will use the feature tomorrow without opening its code.

## 1. Learn what the feature does, from the code

Read, in this order:

1. The diff of the branch: `git diff origin/main...HEAD`, or the files the user names.
2. The tests of the feature. They state the guarantees (ordering, edge cases, errors) more precisely than the implementation.
3. The headers: Doxygen comments, constants and their values, the library the folder belongs to (its `CMakeLists.txt`).
4. The Linear issue or the pull request description, for the intent only.

Document what the code does on this branch, not what the issue hopes for. A page that promises more than the code sends a teammate debugging the wrong layer.

## 2. Decide what needs documenting

Document anything another team will call, configure or have to respect: a public class or component, a named constant that carries meaning (a rate, a size, a port), a behavior visible from outside (ordering, timing, errors), a new folder, a protocol message, a command or option, a build or run step.

Skip a refactor that changes no behavior, a private helper, a test-only change, a rename. Say so in one line with the reason.

Then look for what the feature made false in the existing pages: a "Not yet written" row in a state table, a "… is being written" sentence in `project-layout.md`, a "does not exist yet" paragraph, a sentence that describes the old behavior. Fix them in the same change: nobody else is looking at them right now.

## 3. Choose the pages

A feature usually touches two pages: the team page, and one of the others.

| Page | Holds | Update it when the feature… |
|---|---|---|
| Team page: `engine.md` today; `network.md`, `server.md` (server and game) and `client.md` once their first feature lands | what exists in the subsystem, how to use it, where to change it | adds or changes anything in that subsystem (almost always) |
| `architecture.md` | the decisions every team follows, each with its reasons and an "Alternative considered" | makes a design choice others must respect: a storage layout, a time model, a coordinate frame |
| `project-layout.md` | the map: libraries, programs, folders, what exists | creates a folder, or makes a planned one exist: update the "… exist; … are being written" sentence of its section |
| `protocol.mdx` | the wire format; byte layouts drawn with `ByteLayout` (`docs/src/components/`) | adds or changes a message, the header, a size or the byte order |
| `README.md` | build, run, test | changes a command, an option or a requirement |

Split the why from the how, as the Components feature did: the reasoning and the rejected alternative go to `architecture.md`; the team page says what exists and how to use it, and links to the reasoning (`The reasoning is in [Architecture](/R-Type/architecture/#components).`). Never explain the same thing on two pages: two copies drift apart within a month.

Internal links carry the site base and the anchor: `/R-Type/engine/#entities-entityregistry`. The build checks them.

## 4. Write in the house style

### Voice

- English, American spelling (`behavior`, `center`, `organized`), like the code and the rules.
- Describe the code in the present tense and the third person. Address the reader as "you" only to guide them, as the team pages do: "This page tells you where to look", a "You want to…" table.
- Say plainly what exists and what does not: "Not yet written", "is being written", "The loop itself does not exist yet". Planned behavior is never written in the present tense as if it worked; a rule the code will have to follow is written with "must".
- No em dash: a colon, a semicolon or a new sentence. No Linear identifiers or pull request numbers.

### One concept, one short section

A concept takes a paragraph or a few bullets: about 50 to 200 words, like the sections already merged. Match the length of the neighboring sections, then cut every sentence a developer from another team would not miss.

Every sentence carries a fact a teammate can rely on: what it does, in which unit, what it guarantees, what happens at the edge, what it never does. The why of a design is told through its consequence, introduced by "so". From `engine.md`:

> `EntityRegistry` creates and destroys entities and recycles the index of a destroyed one. The generation tells apart the successive entities that lived in the same slot, so a handle kept after a destruction never matches the entity that took its place: `isAlive()` is false for it.

The page is not an API reference: the Doxygen comments in the headers are. Leave out each overload, each exception class when their common base is enough, and worked numeric examples.

When using the thing is clearer shown than told, add one short code example (under 15 lines) with real names and designated initializers, like the `ComponentRegistry` example in `engine.md`.

### Names, values, units

- Symbols, files, folders, CMake targets and constants go in backticks.
- A constant is cited by its name, with its value the first time: "`SIMULATION_TICKS_PER_SECOND` (60)". The name is what the reader searches for; the value is what they need now.
- Say where the thing lives: a folder in a table, or in parentheses in the text: "`FixedTimestep` (`rtype_engine`)".
- Every quantity has its unit: logical units, units per second, ticks, bytes.

### Provisional values

A value that is expected to change is marked **provisional** in bold, with what will revise it and what code must do until then:

> These dimensions are **provisional**: 800 × 600, the current size of the client window. They will be revised once the proportions of the game are decided. Code must read the constants and never assume they match the window.

### Structure

- Headings name the concept; the class may follow after a colon: `## Game loop: time`, `### Entities: \`EntityRegistry\``. The page title is the only first-level heading (it comes from the frontmatter); then `##` and `###`, no level skipped.
- Tables for parallel facts: a state table (Part, State, Where), a "Where to intervene" table (You want to…, Look at), a decision table (Question, If yes).
- In `architecture.md`, a rejected option gets one paragraph starting with "Alternative considered:", saying why it is simpler and why it loses.
- Diagrams follow `.claude/rules/fix/format/fix-format-schematic-diagrams-and-map-level-pages.md`: one question, a handful of short boxes, one direction, and none rather than a confusing one. A new Mermaid diagram also carries `accTitle` and `accDescr`, so a screen reader gets the same information.

## 5. Templates

### Section added to a team page

Add the section where the concept belongs (not at the end), then update the page's tables: the state table row of the part ("Not yet written" → "Available", with its folder) and a "Where to intervene" row for the new place to change.

### New team page

Mirror `engine.md`. The file name is the slug: `network.md` is served at `/R-Type/network/`. Take the names of the parts from the matching section of `project-layout.md`, so both pages call them the same way.

````markdown
---
title: Network
description: What the network library does, how its parts fit together and where to find them in the code.
---

<One paragraph: what the subsystem is responsible for, what it knows nothing about, and a link to [Architecture](/R-Type/architecture/).>

This page tells you where to look when you need to change something. It describes what exists today and says plainly what does not exist yet.

| Part | State | Where |
|---|---|---|
| <Part> | Available | `src/network/<Folder>/` |
| <Part> | Not yet written | — |

## <Concept>: `<Class>`

<The feature, in the house style.>

## Where to intervene

| You want to… | Look at |
|---|---|
| <Change something> | `src/network/<Folder>/` |
````

### Decision added to `architecture.md`

````markdown
## <Concept>

<The decision and what it guarantees, in one paragraph.>

Alternative considered: <the option>. It is <simpler, faster to write…>, but <why it loses>.
````

A page the four teams must approve (a decision page, the protocol) ends with the `## Validation` table of `architecture.md`, every team `pending`.

### README

Sections stay in their order (Requirements, Build, Tests, Code style, Documentation). A command goes in its own `bash` block, followed by one sentence on what it produces and where.

## 6. Check before handing back

1. Every symbol, constant, value, file and folder the text names exists with that spelling and value: search for each one (`grep -rn`). A wrong name in the documentation is worse than none, because readers trust it.
2. Build the site: `npm ci` in `docs/` if `docs/node_modules` is missing, then `npm run build` in `docs/`. The exit code must be 0 and the log must say "All internal links are valid". Read the exit code without a pipe.
3. Reread the first paragraph of every section you touched as someone from another team: it must say what the thing is and where it lives without the rest of the page.

## 7. Report

Tell the user, in a few lines:

- the pages touched, and the place in each;
- every sentence changed in text that already existed, before and after;
- what was deliberately left undocumented, and why;
- the line for the pull request's `## Changes`, for example: `` `docs/src/content/docs/engine.md`: new "Systems" section, state table updated. ``, and that `Documentation` must be ticked in `## Type of Change`.

The documentation goes in the same pull request as the feature, so the site never describes code that is not on `main`. Commit it on its own (`docs: document <the feature>`) unless the user prefers it in the feature commit.
