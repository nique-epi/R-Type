# RULE: Never reason about the content of a file that was not actually read

The rule was imported from another project when R-Type started; its original incident is not recorded here. This file holds the incidents met on R-Type.

## Update (2026-10-16) — A design document describes the rendering it read, not the one it expects

### Context

Design system for the options menu (RTY-106), written before any code: tokens, widgets and a brand book describing how the client draws its 480 × 270 interface picture.

### Mistake

The brand book said "the window enlarges the picture by an integer factor: ×2 at 960 × 540, ×4 at 1920 × 1080". Nothing had been read about the scaling; the two examples happened to be multiples of 480. Reading `IntegerScale.hpp` and `client.md` showed the real behavior: the largest whole factor that fits (×2 at 1280 × 720, ×3 at 1600 × 900), then the remainder smoothed. Caught before the user read it, corrected in the brand book and in a token's usage note.

### Root cause

The examples were taken from the two sizes that fit the expectation, and the other rows of `WINDOW_SIZES` were never checked against the claim.

### Rule

1. Before writing how the client draws in a page, a design system, a mockup or a plan, open the function that does it and write its behavior with its name.
2. A round example is checked against every row of the table it comes from.

### Example

- ❌ **Before (wrong)**: "the window enlarges the 480 × 270 picture by an integer factor" → 1280 × 720 is 2.67 times 480 × 270.
- ✅ **After (right)**: read `integerScaleFactor` → "the largest whole factor that fits (×3 at 1600 × 900), then only the remainder is smoothed".
