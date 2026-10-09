---
description: A measurement only proves a fact if it would have shown something ELSE in the opposite case
trigger: always_on
---

# RULE: A measurement only proves a fact if it would have shown SOMETHING ELSE in the opposite case

## Rule

1. **Before citing a measurement as evidence, ask what it would show if the fact were false.** If the answer is "the same" or "I don't know", it is not evidence.
2. **An unknown metric is calibrated on a known case** before being read on the case to prove.
3. **Prefer structural evidence to a derived metric**: "the test fails on the old code and passes on the new one" proves it catches the bug; "the test passes" proves nothing.
4. **When evidence already announced turns out not to discriminate, say so** in the next message, with the evidence that replaces it.

### A status page saying "operational" does not prove a service is up

1. A provider's status page lags behind its outages: "All Systems Operational" read during a failure never rules out a failure on the provider's side, and is never listed among the causes checked.
2. When a remote service keeps answering with a server error (5xx) whatever the request, suspect its side first: make one independent call on another resource (another branch, another endpoint) before varying my own request, and read the status page again a few minutes later.

## Example

- ❌ **Before (wrong)**: "The leak fix is validated: the server runs 10 minutes without crashing." (It already ran 10 minutes without crashing before.)
- ✅ **After (right)**: "Before the fix, server memory grows by 2 MB per minute with 4 clients; after, it stays flat over 10 minutes, same load."
- ❌ **Before (wrong)**: `git push` refused with "Internal Server Error" eight times; "the GitHub status page shows no incident" reported as checked; every test varies the commit, the transport or the way the branch is created, all on the same branch.
- ✅ **After (right)**: after the second 500, push another branch and read the status page again → every push fails, the page now shows a Git operations outage → wait and retry, nothing to change on my side.
