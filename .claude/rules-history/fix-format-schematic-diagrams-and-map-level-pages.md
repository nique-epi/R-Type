# RULE: A documentation diagram is schematic, and a page about structure is a map, not an inventory

## Context

The user asked for a clear file layout of the project, with a complete diagram and Markdown, so that the team could code without hesitating about where a file goes. The deliverable was a page of the documentation site (`docs/src/content/docs/project-layout.md`) with five Mermaid diagrams and trees of every library and program.

## Mistake

The first version tried to be complete instead of clear:

- the "big picture" flowchart put seven boxes on the page, each listing up to eleven folders and the third-party libraries in italics, inside subgraphs, with arrows crossing between programs and libraries;
- the "which library?" decision tree held three-line questions in diamonds, which Mermaid draws huge, and ran over a screen and a half;
- the threads and master diagrams had arrows going back up to their origin;
- the trees listed every class expected in every folder, with a status on each line, and a "where does this code go?" table ran to about fifty rows.

The user found the first diagram incomprehensible, the decision tree ugly, and the page boring: either useless information or badly presented.

## Root cause

"Complete diagram" was read as "a diagram that holds everything", when a diagram is useful precisely because it leaves things out. The page was written from what was known (every class of the architecture notes, every issue of the backlog) rather than from what a developer needs before adding a file. No reference on how good architecture pages are written was consulted before writing, and the rendering was checked for syntax errors, not for readability at the width of the site.

## Rule

### Diagrams

1. One diagram answers one question, written in the sentence just above it.
2. About seven nodes at most, one to four words each; details in the text or a table under it.
3. Every arrow goes the same way; a flow that comes back is a sequence diagram with four or five participants.
4. Questions asked in order go in a table, not in diamonds.
5. Check the rendering at the width of the site, at 100 %; cut or split what needs zooming, crosses or exceeds a screen.
6. No diagram is better than a confusing one.

### Pages about structure

1. Map level: coarse modules, what each holds, what it never holds, where to look.
2. Each module in a few lines: role, invariant, a tree one or two levels deep.
3. One worked example of a convention rather than a table of every rule.
4. Read how reference documentation does it before writing a new kind of page.

## Example

- ❌ **Before (wrong)**: a "big picture" flowchart of seven boxes each listing ten folders, a decision tree with three-line diamonds, trees of every expected class, a fifty-row table.
- ✅ **After (right)**: dependency rules in five bullets, a five-row "where does a new class go" table, a role, an invariant and a two-level tree per library, one four-participant sequence diagram for a key press.
