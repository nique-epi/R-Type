# History: French in the conversation and in pull request comments

## Context
On 2026-10-27 the user asked that Claude speak only French, and that comments on pull requests be in French too.

## Mistake
None: a request, not an incident. Until then `fix-format-pull-requests-in-english.md` required every comment posted on a pull request to be in English, and no rule fixed the language of the conversation.

## Root cause
The language rules only covered what lives in the repository (code, commits, pull requests), and counted pull request comments among them.

## Rule
See `.claude/rules/fix/format/fix-format-french-in-the-conversation-and-pr-comments.md`. Comments posted on a pull request moved from the English rule to this one; the title and description of a pull request stay in English.
