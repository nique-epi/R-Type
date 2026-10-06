---
description: "Documentation diagrams are schematic (one question, a handful of short boxes, one direction) and a structure page is a map, not an inventory"
trigger: always_on
---

# RULE: A documentation diagram is schematic, and a page about structure is a map, not an inventory

## Rule

### Diagrams

1. **One diagram answers one question**, written in the sentence just above it ("which thread runs what", "the path of a key press").
2. **A handful of boxes, short labels.** About seven nodes at most, one to four words each. No list, file tree, technology note or italics inside a node: details go in the text or in a table under the diagram.
3. **Every arrow goes the same way.** A flow that comes back (request and reply, server then client again) is a sequence diagram with four or five participants, never a flowchart with arrows going back up.
4. **No decision tree with long questions in diamonds.** Questions asked in order go in a table: question, answer if yes.
5. **Check it rendered at the width of the site, at 100 %.** Text that needs zooming, crossing arrows or a diagram taller than a screen: cut or split it before shipping.
6. **No diagram is better than a confusing one.** If the boxes and arrows say nothing the text does not already say better, drop the diagram.

### Pages about structure (layout, architecture, codemap)

1. **Map level, not atlas level.** Name the coarse modules, what each holds, what it never holds (its invariants), and where to look. No list of every future class, no table of fifty cases.
2. **Each module in a few lines**: a one-sentence role, its invariant, a folder tree one or two levels deep with a short comment per line.
3. **Show one worked example** of a convention rather than a table of every rule.
4. **Before writing a kind of page not yet written in this project**, read how reference documentation does it (ARCHITECTURE.md codemap, C4 diagram checklist, a mature project's architecture page) and follow that shape.

## Example

- ❌ **Before (wrong)**: a "big picture" flowchart of seven boxes each listing ten folders and their third-party libraries, plus a decision tree whose diamonds hold three-line questions; then trees listing every expected class, and a fifty-row table.
- ✅ **After (right)**: the dependency rules in five bullets; "where does a new class go" as a five-row table; per library a role, an invariant and a two-level tree; one sequence diagram with four participants for the path of a key press.
