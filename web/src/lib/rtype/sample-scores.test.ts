import { describe, expect, it } from 'vitest';

import { BOARD_SIZE, MONTHS, sampleScores } from './sample-scores';

describe('sampleScores', () => {
  it('gives ten ranked pilots, best first', () => {
    const rows = sampleScores(1, 1);

    expect(rows.map(row => row.rank)).toEqual([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
    expect(rows).toHaveLength(BOARD_SIZE);
    expect(rows.map(row => row.score)).toEqual(
      [...rows.map(row => row.score)].sort((a, b) => b - a),
    );
  });

  it('gives the same pilots for the same seed', () => {
    expect(sampleScores(7, 0)).toEqual(sampleScores(7, 0));
  });

  it('gives another board for another seed', () => {
    expect(sampleScores(7, 0)).not.toEqual(sampleScores(8, 0));
  });

  it('puts a pilot once on a board', () => {
    const names = sampleScores(3, 2).map(row => row.name);

    expect(new Set(names).size).toBe(names.length);
  });

  it('dates every score within the months it knows', () => {
    for (const row of sampleScores(5, 1)) {
      expect(row.month).toBeGreaterThanOrEqual(0);
      expect(row.month).toBeLessThan(MONTHS);
      expect(row.day).toBeGreaterThanOrEqual(1);
      expect(row.day).toBeLessThanOrEqual(28);
    }
  });
});
