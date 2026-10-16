# RULE: In zsh, unquoted `?`, `*`, `[` and a leading `=` are EXPANDED — and a failed expansion stops the whole line

This rule came with the rules imported when R-Type started; its first incident belongs to another repository. This file starts with its first update on this project.

## Update (2026-10-13) — an intended glob that may match nothing

### Context

Gates of the title screen work: clang-tidy was to run on every changed file, one log per file, in a scratchpad folder created by the same command.

### Mistake

The command was `mkdir -p "$S/tidy" && rm -f "$S/tidy"/*.txt && git diff --name-only … > "$S/tidy/files.txt"; … grep -v '^0 ' "$S/tidy/exits.txt"`. The folder was new, so `*.txt` matched nothing: zsh printed `no matches found` and the `&&` chain stopped before the file list was written. The commands after `;` still ran and printed an empty "files with nonzero exit:" list.

### Root cause

The rule covered globs written by accident (URLs, filters), not a glob written on purpose over a folder that can be empty. `rm -f` suggests "never fails", but in zsh the expansion fails before `rm` runs.

### Rule

See the core file, subsection "An intended glob that may match nothing".

### Example

- ❌ `rm -f "$S/tidy"/*.txt` on an empty folder.
- ✅ `find "$S/tidy" -name '*.txt' -delete`.

## Update (2026-10-15) — the value of an option, and a bare separator

### Context

Connection screen of the client, after a context reset: reading the files to rework before coding a new design.

### Mistake

Two calls in a row failed on expansion. The first held `grep -rn … --include=*.cpp --include=*.hpp`: zsh printed `no matches found: --include=*.cpp`. The next held `cat <file>; echo; echo =====; grep … BackgroundColors.hpp`: zsh printed `==== not found`, so the grep after it never ran.

### Root cause

Point 1 lists "any argument", but `--include=*.cpp` reads as an option, not as a glob; point 2 gives `echo '-----'`, yet a separator was typed from habit without quotes.

### Rule

See the core file, subsection "The value of an option and a separator are quoted like any other word".

### Example

- ❌ `grep -rn FIELD_SIZE src --include=*.cpp; echo =====; grep …`.
- ✅ `grep -rn FIELD_SIZE src --include='*.cpp'; echo '-----'; grep …`.
