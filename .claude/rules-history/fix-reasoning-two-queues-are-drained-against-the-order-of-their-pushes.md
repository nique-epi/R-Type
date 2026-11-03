# RULE: Two queues are drained against the order of their pushes

## Context

The storage thread of the server holds two `BoundedQueue`: the results of the games, and the requests for a leaderboard. Its doc comment promised that a request made after a result reads that result.

## Mistake

The first version of `ResultsArchive::workThrough` drained the results, saved them, then drained the requests. The test `AnswersOnTheQueueOfTheAskAfterSavingWhatCameFirst` could fail only when both pushes fell between the two drains, which is rare.

## Root cause

The two queues were read in the order they were pushed to, which feels natural. The interleaving was not walked by hand. It was found while rereading the plan, before any commit.

## Rule

1. **When a consumer must see A before it serves B, and a producer pushes to A then to B, it drains B first, then A, then works through A, then through B.**
2. **Walk the interleaving by hand before writing it.**
3. **Say the order and its reason in the doc comment of the consumer.**

## Example

- ❌ **Before (wrong)**: drain the results, save them, then drain the requests.
- ✅ **After (right)**: drain the requests, then the results, save the results, then answer the requests.
