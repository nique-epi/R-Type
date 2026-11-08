import { describe, expect, it } from 'vitest';

import { LOOK_ITEMS, lookItemSourceOf } from './ship-looks';

describe('lookItemSourceOf', () => {
  it('puts the top hat, the cool shades and the mustache in the starter kit', () => {
    expect(lookItemSourceOf('topHat')).toBe('starter');
    expect(lookItemSourceOf('coolShades')).toBe('starter');
    expect(lookItemSourceOf('mustache')).toBe('starter');
  });

  it('brings the crown in any mission and the wizard hat only in a normal or hard one', () => {
    expect(lookItemSourceOf('crown')).toBe('mission');
    expect(lookItemSourceOf('wizardHat')).toBe('harderMission');
  });

  it('leaves seven items to win in a mission, as the game does', () => {
    const won = LOOK_ITEMS.filter(item => lookItemSourceOf(item) !== 'starter');
    expect(won).toHaveLength(7);
  });
});
