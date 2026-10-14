# History: a sanitizer sees only what leaves the allocation

## Context
The game sprites work added a guard to `RenderSystem::render`: a sprite whose layer is past the last drawing layer is skipped instead of being pushed into `drawCallsByLayer_[layer]`, out of the bounds of that `std::array`. A review of an earlier pull request had measured that out-of-bounds write under ASan. The test `SkipsASpriteWhoseLayerIsNotADrawingLayer` uses the test fixture `Scene`, which holds `RenderSystem system_;` followed by `RecordingSurface surface_;`.

## Mistake
The plan predicted that, with the guard removed, the ASan build would report the overflow. The red proof ran on an ASan build: the test failed, but ASan reported nothing.

## Root cause
`drawCallsByLayer_` is the last member of `RenderSystem`, and `RenderSystem` is followed by another member inside `Scene`, so index 4 writes into memory that still belongs to the same object. ASan puts its red zones around allocations, not between the members of an object, so it cannot see that write. The earlier measurement came from another layout and was carried over without checking.

## Rule
See `.claude/rules/fix/reasoning/fix-reasoning-a-sanitizer-sees-only-what-leaves-the-allocation.md`.

## Example
- ❌ **Before (wrong)**: "without the layer guard, ASan reports the overflow".
- ✅ **After (right)**: "the write stays inside `Scene`, so ASan cannot see it; the test turned red through corrupted data, undefined behavior".
