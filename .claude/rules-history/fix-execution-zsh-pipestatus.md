# RULE: Getting the exit code of a piped command in zsh (not `PIPESTATUS`)

This rule was imported from another project when R-Type started; its original incident is not recorded here. This history starts with its first recurrence in this repository.

## Update (2026-10-21) — `echo $?` after `| head` in an absence check

### Context

Checking, for the RTY-122 effects proposal, that the server never runs the collision rules and that the client never decodes `GameEvent` nor reads `shipEntityId`.

### Mistake

The checks were written `git grep -n '<pattern>' -- <paths> | head -5; echo "…: exit $?"`. Each printed `exit 0`, the code of `head`, while `grep` had found nothing. The empty output was read correctly, and the checks were rerun without a pipe (`git grep -q`, exit 1 each) before anything was concluded; but the printed lines were evidence that said the opposite of the truth.

### Root cause

The forbidden shape of point 1 of the rule, written out of habit to cap the output.

### Rule added

The subsection "An absence is proven by grep's own exit code" of the core rule.
