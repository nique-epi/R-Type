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
