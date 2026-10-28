---
description: Language — French in the conversation and in every comment posted on a pull request; what lives in the repository stays in English
trigger: always_on
---

# RULE: French in the conversation and in pull request comments — the repository stays in English

Why: the user asked for it.

## Rule

1. **Everything said to the user is in French**: answers, questions, plans, reports, relayed reviews and agent reports, the end-of-turn summary. This holds even when the user, a tool, a page or an agent writes in English.
2. **Every comment posted on a pull request is in French**: review comments, inline comments on a line, replies to a teammate or a bot, follow-ups after a push.
3. **What lives in the repository stays in English**: code and its comments, commit messages, the title and description of a pull request, the rules, the docs site and the README (see `core/commit.md`, `fix-format-pull-requests-in-english.md`, `code-style-comments-english-outside-bodies-no-tickets.md`).
4. **Proper names and identifiers are not translated** in French text: files, branches, CMake targets, symbols, commands, and the severity names of a review (`Critical`, `Warning`, `Info`).
5. **A brief written for a subagent may stay in English**; what it reports is relayed to the user in French.

## Example

- ❌ **Before (wrong)**: a reply to a reviewer on a pull request: "Good catch, fixed in 3f2a1c0: the size is now checked before the copy."
- ✅ **After (right)**: "Bien vu, corrigé dans 3f2a1c0 : la taille est maintenant vérifiée avant la copie."
