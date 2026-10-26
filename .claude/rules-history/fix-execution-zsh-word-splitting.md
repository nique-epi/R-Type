---
description: History of the rule on word splitting in zsh
---

# History: fix-execution-zsh-word-splitting

The rule was imported from another project when R-Type started; its original incident belongs to that repository. The updates below happened here.

## Update (23 October) — `for name in $defined` ran once

### Context
Proposing how to group the files of `src/client/Rendering/Effects/` into family folders, which needed the list of constants and types each file takes from the shared headers, to keep every folder link one way.

### Mistake
The script collected the names a header defines in `defined=$(…)`, then looped `for name in $defined; do grep -q -w "$name" "$file" && echo "$name"; done`. zsh ran the loop once, with every name in `name`; `grep` read each line as a pattern and matched, so every file seemed to use every constant of every header.

### Root cause
A bash reflex: the loop relied on word splitting of an unquoted variable, which zsh does not do. The output repeated the whole list for each file, a sign that went unread before the rerun.

### Rule
Loop over a list with `${(f)list}` or `${=list}`, or match in a Python script that splits explicitly; an output that repeats its whole input is the symptom.
