# RULE: A measurement only proves a fact if it would have shown SOMETHING ELSE in the opposite case

This rule was imported from another project when R-Type started; its original incident belongs to that repository. Only the updates made here are recorded below.

## Update (2026-10-07) — A status page saying "operational" does not prove a service is up

### Context

The rules branch had to be pushed to GitHub and its draft pull request opened, a few minutes after the event bus branch had been pushed without trouble.

### Mistake

Every push of the rules branch was refused with "remote rejected (Internal Server Error)". I read the GitHub status page, which said "All Systems Operational", and reported it to the user among the causes ruled out. I then ran six more attempts, each varying one thing about my own request (only the first commit, a commit already on GitHub, HTTPS instead of SSH, the branch created through the API first), all on the same branch, and concluded that pushes to that branch failed. The user suggested GitHub was down. Pushing the event bus branch then failed the same way, and the status page, read again at that moment, showed a major outage of Git operations, pull requests and Actions.

### Root cause

The status page was treated as a measurement of availability, while it is updated after an outage starts: it would have shown "operational" whether GitHub was failing or not, so it ruled nothing out. The tests varied my request instead of checking first whether any request to the service succeeded.

### Rule

1. A provider's status page lags behind its outages: "All Systems Operational" read during a failure never rules out a failure on the provider's side, and is never listed among the causes checked.
2. When a remote service keeps answering with a server error (5xx) whatever the request, suspect its side first: make one independent call on another resource before varying my own request, and read the status page again a few minutes later.

### Example

- ❌ **Before (wrong)**: eight refused pushes, "the status page shows no incident" reported as checked, every test on the same branch.
- ✅ **After (right)**: after the second 500, push another branch and read the status page again → every push fails, the page shows a Git operations outage → wait and retry.
