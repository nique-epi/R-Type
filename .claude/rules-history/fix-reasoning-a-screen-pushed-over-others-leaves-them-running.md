---
description: "The screen stack updates the screens a pushed screen hides: list what they still do before pushing, and enter with replaceAll a screen every exit of which replaces the stack"
trigger: always_on
---

# RULE: A screen pushed over others leaves them running — what they still do must be decided

## Context

RTY-125: once the session with the server ends, the frame loop shows the NO SIGNAL screen (`LostScreen`) over whatever screen the player was on, the game included. The approved plan pushed it (`screens_.push(ScreenId::Lost)`) so that the screens below stayed drawable for the frozen picture the screen squeezes.

## Mistake

A review pointed out that `ScreenStack::update()` updates every screen, the hidden ones too: under the card, the game screen kept running its systems and could still ask `replaceAll(ScreenId::End)` once a `GameEnded` was due, which would destroy the card; an end screen below would keep playing its sounds and could ask for the waiting room.

## Root cause

The plan kept the screens alive for one need, the picture, and never listed what else a live screen does each frame. Every exit of the NO SIGNAL screen replaces the stack anyway, so nothing below was ever needed after the picture.

## Rule

1. Before pushing a screen over others, list what each screen below still does in its `update()`, and decide whether it may happen under the new screen.
2. A screen every exit of which is `replaceAll()` is entered with `replaceAll()`; what it needs from the screens below is taken before.
3. A `replaceAll()` the frame loop asks for is asked after `screens_.update()`, so a navigation asked in the same frame is applied before it.

## Example

- ❌ `push(ScreenId::Lost)` over the game: the end screen asked by the game replaces the card.
- ✅ `framePictureOf()` after the update, then `replaceAll(ScreenId::Lost)`.
