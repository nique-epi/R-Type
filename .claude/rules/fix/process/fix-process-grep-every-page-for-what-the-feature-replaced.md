---
description: Documenting a feature searches every page and the README for what the feature replaced, not only the pages it is about
trigger: always_on
---

# RULE: Documenting a feature searches every page for what it replaced, not only the pages it is about

Why: the options menu replaced the provisional options screen; `client.md` and `project-layout.md` were updated, but `architecture.md` still described "one bar per size", and `client.md` itself still said "the options screen is provisional", until a later pass.

## Rule

1. **Before calling a feature documented, list the names of what it replaces or changes**: the old screen or class, the words that described it ("provisional", "stands in for", "one bar per size"), a removed constant, a renamed target.
2. **Search the whole site and the README for each of them**: `git grep -n -i -e '<word>' -- docs/src/content README.md`. The pages the feature is about are never the whole list.
3. **Every hit that describes the old behavior is rewritten in the same change**, whichever page it is on.
4. **The report of the documentation step lists the words searched and the hits fixed**, so the reader sees what was looked for.

## Example

- ❌ **Before (wrong)**: the Options section added to `client.md`, the map updated, the site built; `architecture.md:90` still said "one bar per size, a filled check box on the size of the window … The options screen is provisional".
- ✅ **After (right)**: `git grep -n -i -e 'options screen' -e 'check box' -- docs/src/content README.md` → `architecture.md:90` and `client.md:50` ("The options screen is **provisional**") among the hits → both rewritten.
