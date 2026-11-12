import { describe, expect, it } from 'vitest';

import { formatGigabytes, formatMegabytes } from './format-size';

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

describe('formatGigabytes', () => {
  it('writes 3.3 billion bytes with a comma in French and a point in English', () => {
    expect(formatGigabytes(3_300_000_000, 'fr')).toBe('3,3');
    expect(formatGigabytes(3_300_000_000, 'en')).toBe('3.3');
  });

  it('writes a round size without a decimal', () => {
    expect(formatGigabytes(8_000_000_000, 'fr')).toBe('8');
  });
});
