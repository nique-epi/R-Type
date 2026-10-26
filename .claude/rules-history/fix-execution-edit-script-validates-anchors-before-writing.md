
## Update (2026-10-22) — An inline loop that wrote file by file

**Context.** Fixing five failing tests of the visual effects of the client (RTY-122): six edits over six files, typed as an inline Python heredoc, while a batch tool that checks every anchor first (`apply_edits.py`) already existed in the session.

**Mistake.** The loop checked the anchors of a file, then wrote it, then went to the next. The anchor of the fifth file had been reflowed by `clang-format` after the file was extracted from the plan, so the script stopped there: four files written, two not.

**Root cause.** A short inline loop did not register as "an edit script"; and the anchors were copied from the plan, written before the formatter ran.

**Addition to the rule.** An inline loop over several files builds every text first and writes after; the batch tool is used when it exists; anchors are read from the formatted files.

## Update (24 October 2026) — A script that keeps some changes of a file decides line by line

A script built the five commits of a rules pull request from the local rules: for the shared index, it kept the changes whose new lines held a marker of the commit, and reverted the others. It decided per `difflib` block, and the index lines of two rules, added next to each other, formed one block: the first commit carried both. Its check (each marker matched once) passed. Fixed before the commit: the script splits each block into lines. New subsection: decide line by line, and check that the lines kept are exactly the lines the markers name.

## Update (25 October 2026) — Two files written one after the other

Two doc comments, in two headers, were fixed by an inline loop that checked the anchors of a file and wrote it before reading the next one. Both anchors held, so nothing broke, but the second file was never checked before the first was written. New subsection: from two files on, fill the dictionary, then write.
