# History: verification tooling stays on my side

## Context
The user asked for a skill that writes the project documentation in the style of the existing pages. A generic skill-building procedure was followed to create it, and the user accepted, in a multiple-choice question, to validate the skill by replaying past features with and without it.

## Mistake
On top of the six test agents and a grading script, the generic procedure's review viewer was started as a local web server and opened in the user's browser pane, with instructions to leave feedback in it and submit it. The user asked why a web platform had been created: they only wanted the skill.

## Root cause
The generic procedure's steps were applied as written. The test option the user accepted said "you compare the outputs side by side" without saying it meant a local web tool, and nothing checked that the user wanted to review results anywhere other than the conversation.

## Rule
See `.claude/rules/fix/process/fix-process-verification-tooling-stays-on-my-side.md`.
