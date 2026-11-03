# RULE: A multi-step sequence ASSERTS its starting state — it never assumes where the previous step left it

This rule was imported from another project when R-Type started; its original incident belongs to that repository.

## Update (3 November 2026) — Applying the state already in force

The filter of the leaderboard asked the server again when the player confirmed it without changing anything, and the list showed its waiting state, then the same rows: it blinked. Point 1 already asked for an idempotent step ("already in the requested state ⇒ immediate success"); the filter did not compare the choice with the one in force. A review found it; a test that applies the same filter now expects one request, and was seen failing before the fix.
