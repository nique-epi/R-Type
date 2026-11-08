import { describe, expect, it } from 'vitest';

import { formatDay } from './format-date';

describe('formatDay', () => {
  it('writes the day of a release in French', () => {
    expect(formatDay('2026-11-06T21:31:56Z', 'fr')).toBe('6 novembre 2026');
  });

  it('writes it in English', () => {
    expect(formatDay('2026-11-06T21:31:56Z', 'en')).toBe('November 6, 2026');
  });

  it('keeps the day in UTC, whatever the zone of the server', () => {
    expect(formatDay('2026-11-06T23:59:59Z', 'fr')).toBe('6 novembre 2026');
    expect(formatDay('2026-11-07T00:00:00Z', 'fr')).toBe('7 novembre 2026');
  });

  it('writes a short month for a list', () => {
    expect(formatDay('2026-11-06T21:31:56Z', 'fr', 'short')).toBe(
      '6 nov. 2026',
    );
  });
});
