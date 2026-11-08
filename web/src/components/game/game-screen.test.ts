import { describe, expect, it } from 'vitest';

import {
  fittingScale,
  GAME_SCREEN_HEIGHT,
  GAME_SCREEN_WIDTH,
} from './game-screen';

describe('fittingScale', () => {
  it('never draws the screen larger than ×2 of the client', () => {
    expect(fittingScale(GAME_SCREEN_WIDTH * 2)).toBe(1);
  });

  it('shrinks the screen to the width it is given', () => {
    expect(fittingScale(GAME_SCREEN_WIDTH / 2)).toBe(0.5);
  });

  it('takes the height into account when it is the tighter side', () => {
    expect(
      fittingScale(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT * 0.6),
    ).toBeCloseTo(0.6, 9);
  });

  it('grows to fill a wall display when it may upscale', () => {
    expect(
      fittingScale(GAME_SCREEN_WIDTH * 4, GAME_SCREEN_HEIGHT * 4, true),
    ).toBe(4);
  });

  it('fills the tighter side of a display wider than the picture', () => {
    expect(
      fittingScale(GAME_SCREEN_WIDTH * 4, GAME_SCREEN_HEIGHT * 2, true),
    ).toBe(2);
  });

  it('stops at 0.35, below which the box scrolls sideways', () => {
    expect(fittingScale(100)).toBe(0.35);
  });
});
