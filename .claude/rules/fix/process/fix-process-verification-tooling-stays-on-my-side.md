---
description: Verifying a deliverable is the agent's work; the user never gets extra tooling (local web page, viewer, server) they did not ask for
trigger: always_on
---

# RULE: Verification tooling stays on my side — the user gets the deliverable and a plain report

Why: the user asked for a documentation skill and got a local review web page opened in front of them, which read as an unrequested web platform.

## Rule

1. **The user receives what they asked for** (a file, a page, a pull request) **and a short report in the conversation.** Test runs, grading scripts, viewers and local servers are my instruments: they live in the scratchpad and never become something the user has to open, click or submit.
2. **When a step needs the user's judgment, the material comes into the conversation**: an excerpt, a before/after, a table. Never a new tool to learn.
3. **A heavy verification is announced in plain words before it runs**: what runs, how many agents, what the user will have to do (ideally nothing). A "tests" option accepted in a question covers running the tests, not handing their tooling to the user.
4. **A generic skill's procedure does not override this rule**: when a skill says to open a viewer or a dashboard for the user, adapt it to what the user asked for.

## Example

- ❌ **Before (wrong)**: asked for a documentation skill → six test agents, a grading script, and a local "Eval Review" page opened in the browser pane so the user submits feedback there.
- ✅ **After (right)**: the skill file is delivered; the reply gives the test outcome in a few lines (scores, the one weakness found, one excerpt), and the user answers in the chat.
