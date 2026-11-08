import { describe, expect, it } from 'vitest';

import { formatMegabytes } from './format-size';

describe('formatMegabytes', () => {
  it('writes the size of the Linux client of v0.1.0 with a comma in French', () => {
    expect(formatMegabytes(15727096, 'fr')).toBe('15,7');
  });

  it('writes it with a point in English', () => {
    expect(formatMegabytes(15727096, 'en')).toBe('15.7');
  });

  it('writes a round size without a decimal', () => {
    expect(formatMegabytes(2_000_000, 'fr')).toBe('2');
  });
});
