import { describe, expect, it } from 'vitest';

import { enemyScaleFor } from './sheets';

describe('enemyScaleFor', () => {
  it('scales a 24 pixel enemy to a whole number of pixels within the width', () => {
    expect(enemyScaleFor('bydo', 216)).toBe(9);
    expect(enemyScaleFor('bydo', 100)).toBe(4);
  });

  it('scales the 40 pixel voltjelly less than the others', () => {
    expect(enemyScaleFor('voltjelly', 216)).toBe(5);
  });

  it('never draws an enemy smaller than its own pixels', () => {
    expect(enemyScaleFor('voltjelly', 10)).toBe(1);
  });
});
