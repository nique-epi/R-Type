# History: validate against the version the target runs

Rule imported from another project when R-Type started: its original incident belongs to another repository.

## Update (2026-10-06) — latest stable release read on the official source

### Context
Plan for scaling the playfield to the client window, built on the SFML view, viewport and resize event.

### Mistake
The plan cited the SFML API from the pinned version only (3.0.2, read on the tag of the official repository). It did not say which release was the latest stable one, nor whether the API it relied on had changed since. The user asked to go to external sources for the latest stable versions.

### Root cause
"Validate against the target version" was read as "the pinned version is the only one worth reading". The pinned version says what compiles today; it does not say whether the code is about to be outdated, which is what the user wanted to know before approving.

### What was done
The latest stable releases were read on the official repositories (SFML 3.1.0, GoogleTest 1.18.0), the headers and the source file the plan uses were compared between SFML 3.0.2 and 3.1.0 (identical for what the plan uses), and both versions were written in the plan. The user then confirmed that no version was to be changed.
