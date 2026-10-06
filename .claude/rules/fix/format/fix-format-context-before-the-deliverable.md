---
description: Every deliverable (plan, report, review, summary, PR) opens with the context the reader does not have yet in this conversation
trigger: always_on
---

# RULE: Set the CONTEXT before the deliverable — never a summary that assumes what was never discussed

## Rule

1. **Every deliverable — plan, report, review, summary, PR description — opens with a 2 to 5 line context reminder**, as soon as the topic was not discussed earlier **in this conversation**: what it is about, where (client, server, engine, build), the current state, what is wrong.
2. **The criterion is the conversation, not the repository.** A fact discovered in a tool call of this turn is not known to the user: they did not read it.
3. **Context is stated in words, not references.** `PacketReader.cpp:42` is not context, it is **evidence**; it comes after the statement, never in its place.
4. **Name things plainly**: "the server ignores the second player's input packets", not "`handleInput` returns early". The symbol name in parentheses if useful.
5. **A correction is contextualized twice**: recall what was claimed, then what I measured.
6. **Reread test**: would a reader who saw none of my tool calls understand the first paragraph? If not, rewrite the opening.

### A summary of an issue separates what already exists from what the change adds

1. The first line of the context names what the change **adds**, and only that.
2. When the issue lists items that already exist in the code, say so in a table (what the issue asks / state / in this change), never by repeating the issue's list as the scope.
3. Reread test: could a reader believe the change creates something it does not touch? If yes, rewrite.

## Example

- ❌ **Before (wrong)**: "`ClientRegistry::find` returns `end()` for the IPv6-mapped endpoint (ClientRegistry.cpp:31). I normalize in `UdpServer`…"
- ✅ **After (right)**: "**The problem.** On Windows, the second player connects but cannot move: the server does not recognize the address their packets come from, because it arrives in a different form (IPv4 wrapped in IPv6) than at connection time. The measured detail is below."
