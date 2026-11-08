import { describe, expect, it } from 'vitest';

import { formatDuration } from './format-duration';

const WORDS = {
  seconds: '{count} s',
  minutes: '{count} min',
  hours: '{count} h',
};

describe('formatDuration', () => {
  it('counts seconds up to a minute and a half', () => {
    expect(formatDuration(0, WORDS)).toBe('0 s');
    expect(formatDuration(89, WORDS)).toBe('89 s');
  });

  it('counts minutes from a minute and a half to an hour and a half', () => {
    expect(formatDuration(90, WORDS)).toBe('2 min');
    expect(formatDuration(480, WORDS)).toBe('8 min');
    expect(formatDuration(5399, WORDS)).toBe('90 min');
  });

  it('counts hours from an hour and a half', () => {
    expect(formatDuration(5400, WORDS)).toBe('2 h');
    expect(formatDuration(86400, WORDS)).toBe('24 h');
  });
});
