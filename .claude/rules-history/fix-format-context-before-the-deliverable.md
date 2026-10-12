# RULE: Set the CONTEXT before the deliverable — never a summary that assumes what was never discussed

This rule was imported from another project when R-Type started; its original incident belongs to that repository. Only the updates made here are recorded below.

## Update (2026-10-06) — A summary of an issue separates what already exists from what the change adds

### Context

The issue asking for the base components and the movement system listed four components: position, velocity, collision box and entity type. Three of them were already on `main`, added by the coordinate system work; only the entity type and the movement system were missing.

### Mistake

The implementation plan opened with "the issue asks for the base components — position, velocity, collision box, entity type — and a movement system". The state of the code came two lines below. The user read the opening as the scope of the change and asked why velocity was part of it.

### Root cause

The context paraphrased the issue instead of stating the change. The check of the code state had been done (`fix-reasoning-check-code-state-before-scoping.md`), but the way it was written put the issue's list first and the correction after, so the first sentence a reader sees described work the change does not do.

### Rule

1. The first line of the context names what the change **adds**, and only that.
2. When the issue lists items that already exist in the code, say so in a table (what the issue asks / state / in this change), never by repeating the issue's list as the scope.
3. Reread test: could a reader believe the change creates something it does not touch? If yes, rewrite.

### Example

- ❌ **Before (wrong)**: "The issue asks for the base components (position, velocity, collision box, entity type) and a movement system." followed later by "Position, Velocity and CollisionBox already exist."
- ✅ **After (right)**: "**This change adds two things: the `EntityType` component and the `MovementSystem`.**" followed by a table: position, velocity, collision box: already done, untouched; entity type: added; movement system: added.

## Update (2026-10-07) — A question or an explanation to the user defines its terms

### Context

Planning the engine event bus required three design choices from the user: when subscribers are called, when an event published during delivery is delivered, and what the change delivers while the real subscribers do not exist yet.

### Mistake

The three questions were asked with "bus", "subscriber", "dispatch" and "forEach" left undefined. The user dismissed them three times, then asked what a bus is, what "the engine may not depend on the game" means, and what a subscriber is. One explanation compared deferred delivery to "a letter read at a fixed time"; the user understood that the player would see events late, while the fixed time is the end of the same tick, 60 times per second.

### Root cause

The terms were familiar from the code I had just read, so I wrote the questions for a reader who had read it too. The analogy was chosen for the idea of waiting, without checking how long a reader would imagine the wait to be.

### Rule

1. Before asking the user to choose, define every term the options rely on, in plain words, with one example taken from the project.
2. An analogy is checked against what the reader will infer from it (a delay, a cost, a risk). If it suggests a false value, state the real value next to it.
3. A question the user dismisses without answering signals terms they did not understand: explain first, then ask again.

### Example

- ❌ **Before (wrong)**: "Deferred delivery (recommended): `publish()` queues the event; `dispatch()` delivers it, called by the loop after `systems.run()`." asked to a user who never saw the words "bus" or "subscriber" defined.
- ✅ **After (right)**: a short glossary (event, bus, publish, subscriber, delivery, queue, `dispatch()`), one worked example from the game (a missile touches a Bydo, step by step), then the question, stating that both options send the announcement in the same tick.
