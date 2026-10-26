# RULE: Never relay the facts of an agent report without separating what it VERIFIED from what it delegated or assumed

This rule was imported from another project when R-Type started; its original incident belongs to that repository. Only the updates made here are recorded below.

## Update (2026-10-22) — The first sentence is never stronger than the report

### Context

The user asked whether moving a Claude Code session to the cloud consumes cloud credits. An agent was sent to read the official documentation, with the verified / not verified marking required in its brief.

### Mistake

The agent reported that cloud sessions count against the plan's usage limits with no separate compute charge (verified), that the official pages mention no separate cloud credit (not verified), and that third-party posts claim a one-time $100 (Pro) or $250 (Max) cloud credit it could not confirm. My answer opened with "No, there are no separate cloud credits", and only listed the third-party claim at the bottom, under "not verified". The user then showed their usage page: a "Cloud session credits" block, applied automatically to cloud sessions, with an included credit and an expiry date.

### Root cause

The body of the answer kept the marking, but the opening turned "the documentation does not mention it" into "it does not exist". The documentation was the only place searched, while an included credit is a property of the account, visible on the user's own usage page. The third-party source that pointed at the real answer was treated as noise because it was not official.

### Rule

1. The opening of the answer carries the weakest verdict it rests on. When the report marks a point "not verified" or "the docs do not say", the first sentence says "the documentation does not mention it", never "there is none".
2. An absence in the documentation is not an absence in the product. What depends on the account can exist only on the user's own settings page: name that page as the place that settles it, or ask the user to look.
3. A third-party source that contradicts the docs' silence is a lead: it goes in the opening, next to the silence.

### Example

- ❌ **Before (wrong)**: "No, there are no separate cloud credits", with the third-party $250 claim at the bottom; the usage page shows a cloud session credit.
- ✅ **After (right)**: "The documentation does not mention a separate credit; third-party posts mention a one-time credit. Your usage page settles it: if it shows a cloud session credit, that is used first."
