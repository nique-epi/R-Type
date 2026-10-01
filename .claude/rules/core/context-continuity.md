---
description: Context continuity — keep .context/context.md up to date as you go, as the resume point
trigger: always_on
---

# RULE: Context continuity

## Goal

The resume point of the work must be **up to date at all times**, not only at the end of a conversation. The next session — or a context reset mid-session — must be able to resume **without re-reading or re-exploring** the same files. Every rediscovery is a cost paid twice.

On this project the resume point is **`.context/context.md`**. It is local to each developer (ignored by git): it describes **my** session, not the state of the team.

## Rules

- **Cadence**: update `.context/context.md` **as you go** — after each atomic commit or logical unit of work, at each architecture decision, when you learn how a subsystem works (engine, ECS, network protocol, build), when a PR is opened or merged, when a bug is fixed, and when the user asks to pause. Continuity written only at the end is lost if the session is interrupted before.
- **Content**:
  - **Where we are**: current branch, step, status;
  - **What was done**: commits, files touched, decisions made;
  - **Next step**: the exact command or action to resume;
  - **Key files** of the current work;
  - **Pending**: what is unfinished or waiting on the user;
  - **Important decisions** that commit the future.
- **Compaction**: past ~150 lines, archive finished steps into a "History" section (2-3 lines each), keep the current step detailed, keep every pending item and every binding decision.
- **Format**: scannable — tables for file lists, bullets for decisions, code blocks for commands. No long paragraphs.
- **What concerns the team does not live here**: a shared architecture decision goes into the repository docs or the PR, not only into an ignored file.

## Examples

- ❌ **Forbidden**: chaining five commits then updating `.context/context.md` all at once at the end (or not at all) → an interruption at commit 3 loses the thread.
- ✅ **Instead**: after each meaningful step, reflect progress in `.context/context.md` **before** moving on.
