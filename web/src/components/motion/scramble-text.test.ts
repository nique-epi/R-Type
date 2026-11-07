import { describe, expect, it } from 'vitest';

import { scrambled } from './scramble-text';

const ALWAYS_FIRST = () => 0;

describe('scrambled', () => {
  it('keeps the settled characters and mixes the rest', () => {
    expect(scrambled('rtk_live_ab', 4, 'x', '_', ALWAYS_FIRST)).toBe(
      'rtk_xxxx_xx',
    );
  });

  it('never mixes a kept character', () => {
    expect(scrambled('a_b', 0, 'x', '_', ALWAYS_FIRST)).toBe('x_x');
  });

  it('is the text itself once every character is settled', () => {
    expect(scrambled('rtk_live', 8, 'x', '_', ALWAYS_FIRST)).toBe('rtk_live');
  });
});
