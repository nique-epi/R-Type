import { describe, expect, it } from 'vitest';

import { format, formatCount, selectCountTemplate } from './format';

const PILOTS = { one: '{count} pilote', other: '{count} pilotes' };
const PILOTS_EN = { one: '{count} pilot', other: '{count} pilots' };

describe('format', () => {
  it('fills every hole of the template', () => {
    expect(
      format('{pilots}/{capacity} PILOTES', { pilots: 3, capacity: 32 }),
    ).toBe('3/32 PILOTES');
  });

  it('leaves a hole without a value as written', () => {
    expect(format('{name} est créé', {})).toBe('{name} est créé');
  });
});

describe('formatCount', () => {
  it('says "0 pilote" in French, where 0 takes the singular', () => {
    expect(formatCount(0, PILOTS, 'fr')).toBe('0 pilote');
  });

  it('says "0 pilots" in English, where 0 takes the plural', () => {
    expect(formatCount(0, PILOTS_EN, 'en')).toBe('0 pilots');
  });

  it('takes the singular for one and the plural for two in both languages', () => {
    expect(formatCount(1, PILOTS, 'fr')).toBe('1 pilote');
    expect(formatCount(2, PILOTS, 'fr')).toBe('2 pilotes');
    expect(formatCount(1, PILOTS_EN, 'en')).toBe('1 pilot');
    expect(formatCount(2, PILOTS_EN, 'en')).toBe('2 pilots');
  });
});

describe('selectCountTemplate', () => {
  const templates = { one: '{count} pilote', other: '{count} pilotes' };

  it('gives the template of the count, unfilled', () => {
    expect(selectCountTemplate(1, templates, 'fr')).toBe('{count} pilote');
    expect(selectCountTemplate(3, templates, 'fr')).toBe('{count} pilotes');
  });

  it('reads zero as French does, as a singular', () => {
    expect(selectCountTemplate(0, templates, 'fr')).toBe('{count} pilote');
    expect(selectCountTemplate(0, templates, 'en')).toBe('{count} pilotes');
  });
});
