
## Update (2026-10-22) — An inline loop that wrote file by file

**Context.** Fixing five failing tests of the visual effects of the client (RTY-122): six edits over six files, typed as an inline Python heredoc, while a batch tool that checks every anchor first (`apply_edits.py`) already existed in the session.

**Mistake.** The loop checked the anchors of a file, then wrote it, then went to the next. The anchor of the fifth file had been reflowed by `clang-format` after the file was extracted from the plan, so the script stopped there: four files written, two not.

**Root cause.** A short inline loop did not register as "an edit script"; and the anchors were copied from the plan, written before the formatter ran.

**Addition to the rule.** An inline loop over several files builds every text first and writes after; the batch tool is used when it exists; anchors are read from the formatted files.

## Update (24 October 2026) — A script that keeps some changes of a file decides line by line

A script built the five commits of a rules pull request from the local rules: for the shared index, it kept the changes whose new lines held a marker of the commit, and reverted the others. It decided per `difflib` block, and the index lines of two rules, added next to each other, formed one block: the first commit carried both. Its check (each marker matched once) passed. Fixed before the commit: the script splits each block into lines. New subsection: decide line by line, and check that the lines kept are exactly the lines the markers name.

## Update (25 October 2026) — Two files written one after the other

Two doc comments, in two headers, were fixed by an inline loop that checked the anchors of a file and wrote it before reading the next one. Both anchors held, so nothing broke, but the second file was never checked before the first was written. New subsection: from two files on, fill the dictionary, then write.

## Update (23 November 2026): a rewrite of a span and a cut at a word of a comment

**Context.** The weapons of the ship (RTY-271): two inline Python scripts edited sources and sheets.

**Mistake.** The first sorted the quoted includes of `WeaponSystem.cpp` by replacing every line from the first to the last `#include "` with the sorted quoted ones, and dropped the six standard includes that sat between them; the build passed through other headers, and clang-tidy caught it before the commit. The second swapped the `[appearance]` of seven sheets by cutting each file at the first `[appearance]`, which the header comment names; every `[gameplay]` was cut, seen on the diff before the commit.

**Root cause.** Both scripts located a place by the first and last match of a text, without checking that the span held only what they meant to replace, and wrote without comparing what they kept.

**Addition to the rule.** Insert at one place and compare the lines of the kind touched before and after; cut at a heading on its own line and assert the kept part before writing.

