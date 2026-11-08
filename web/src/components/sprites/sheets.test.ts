import { describe, expect, it } from 'vitest';

import { enemyScaleFor, lookItemSprite } from './sheets';

describe('enemyScaleFor', () => {
  it('scales a 24 pixel enemy to a whole number of pixels within the width', () => {
    expect(enemyScaleFor('bydo', 216)).toBe(9);
    expect(enemyScaleFor('bydo', 100)).toBe(4);
  });

  it('scales the 40 pixel voltjelly less than the others', () => {
    expect(enemyScaleFor('voltjelly', 216)).toBe(5);
  });

  it('scales the 128 pixel Tyrant to one pixel of the sheet per pixel in a panel', () => {
    expect(enemyScaleFor('bydoTyrant', 216)).toBe(1);
  });

  it('never draws an enemy smaller than its own pixels', () => {
    expect(enemyScaleFor('voltjelly', 10)).toBe(1);
  });
});

describe('lookItemSprite', () => {
  it('reads the crown from the third cell of looks.png, 18 pixels apart', () => {
    expect(lookItemSprite('crown').frame).toEqual({
      x: 36,
      y: 0,
      width: 12,
      height: 8,
    });
  });

  it('sits the mustache under the nose of the ship, as the client does', () => {
    expect(lookItemSprite('mustache').mount).toEqual({ x: 20, y: 9 });
  });
});
