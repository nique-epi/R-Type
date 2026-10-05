# History: identifiers proposed outside the code follow the code naming rules

## Context
While designing the part 2 architecture (several game servers, a master service, a web client), class names, protocol message names, wire fields and folders were proposed in an architecture page and in new Linear issues.

## Mistake
The proposals used abbreviations the project forbids in identifiers: `ServerInfo`, `AdminController`, `AdminConsole`, `AckTracker`, `ack` and `ack_bits` as wire fields, and a `repos/` folder. Three issues were created with `ServerInfo` before the mistake was noticed.

## Root cause
`code-style-full-names-no-abbreviations.md` is scoped to `**/*.{cpp,hpp,tpp}`: it is only loaded when a C++ file is read. Names proposed in documents and issues were never checked against it, although they are the names the code will use.

## Rule
See `.claude/rules/fix/format/fix-format-identifiers-proposed-outside-code.md`.
