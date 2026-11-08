import { describe, expect, it } from 'vitest';

import { bubbleBox, rowsOf } from './sector-bubble';

const line = (text: string) => ({ text, tone: 'plain' as const });

describe('rowsOf', () => {
  it('counts one row for a line that fits', () => {
    expect(rowsOf([line('75 MS - V0.1.0')])).toBe(1);
  });

  it('wraps a line at its words, as the bubble of a server with 32 pilots does', () => {
    // 21 letters, which the inside of the bubble does not hold on one row.
    expect(rowsOf([line('0/32 PILOTS - 0 GAMES')])).toBe(2);
    expect(rowsOf([line('CLICK OR ENTER: ZOOM IN')])).toBe(2);
  });

  it('adds the rows of every line', () => {
    expect(
      rowsOf([line('0/32 PILOTS - 0 GAMES'), line('75 MS - V0.1.0')]),
    ).toBe(3);
  });

  it('does not split a word that is longer than a row', () => {
    expect(rowsOf([line('ABCDEFGHIJKLMNOPQRSTUVWXYZ')])).toBe(1);
  });
});

describe('bubbleBox', () => {
  const PLACE = { x: 100, y: 100 };

  it('is taller by a row for each row a line wraps into', () => {
    const fits = bubbleBox(PLACE, [line('75 MS - V0.1.0')]);
    const wraps = bubbleBox(PLACE, [line('0/32 PILOTS - 0 GAMES')]);

    expect(wraps.height - fits.height).toBe(16);
  });
});
