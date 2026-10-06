# History: check the delivered artifact, not its footprint

The rule was imported from earlier projects; its original incident is not recorded here.

## Update (2026-10-06): a PR state stated without reading it

### Context
The user had been asked whether to keep or close a draft PR that held a corrective rule. They answered "go" for every open point.

### Mistake
The answer said the PR was kept, reading "go" as "keep". The user had closed the PR and deleted its branch about forty-five minutes earlier; the PR state was not read before writing it.

### Root cause
The state of the PR was taken from the conversation (the question still listed it as open) instead of from GitHub.

### Rule
A PR's state is read with `gh pr view` in the same turn before it is stated, and before acting on an answer about it.
