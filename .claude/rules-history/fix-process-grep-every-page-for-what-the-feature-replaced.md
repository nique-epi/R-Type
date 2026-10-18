# RULE: Documenting a feature searches every page for what it replaced, not only the pages it is about

## Context

Options menu of the client, which replaced the provisional options screen (one bar per window size). The `document-feature` skill was run on the finished feature with the pages the plan named: `client.md` (new Options, Settings and row-control sections) and `project-layout.md`, plus the README. The site built and every internal link was valid.

## Mistake

`architecture.md` still described the provisional screen ("one bar per size, labeled with the size it gives, a filled check box on the size of the window … **The options screen is provisional**"). It surfaced only during the `/simplify` pass, when the unused check box of bars was being removed and its mentions searched; the search written into this rule then found a second one in `client.md` itself, the page the feature was documented on: "The options screen is **provisional**: it stands in for the options menu."

## Root cause

The documentation step updated the pages the plan listed and trusted that list. The skill asks to look for what the feature made false in the existing pages, but no search over the whole site was run, and a link check cannot see a sentence that is merely out of date.

## Rule

1. **Before calling a feature documented, list the names of what it replaces or changes**: the old screen or class, the words that described it ("provisional", "stands in for", "one bar per size"), a removed constant, a renamed target.
2. **Search the whole site and the README for each of them**: `git grep -n -i -e '<word>' -- docs/src/content README.md`. The pages the feature is about are never the whole list.
3. **Every hit that describes the old behavior is rewritten in the same change**, whichever page it is on.
4. **The report of the documentation step lists the words searched and the hits fixed**, so the reader sees what was looked for.

## Example

- ❌ **Before (wrong)**: the Options section added to `client.md`, the map updated, the site built; `architecture.md:90` still said "one bar per size, a filled check box on the size of the window … The options screen is provisional".
- ✅ **After (right)**: `git grep -n -i -e 'options screen' -e 'check box' -- docs/src/content README.md` → `architecture.md:90` and `client.md:50` ("The options screen is **provisional**") among the hits → both rewritten.
