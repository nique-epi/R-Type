import { describe, expect, it } from 'vitest';

import { negotiateLocale } from './negotiate-locale';

describe('negotiateLocale', () => {
  it('gives English to a reader whose browser asks for it', () => {
    expect(negotiateLocale('en-US,en;q=0.9')).toBe('en');
  });

  it('gives French to a reader whose browser asks for French', () => {
    expect(negotiateLocale('fr-FR,fr;q=0.9,en;q=0.8')).toBe('fr');
  });

  it('follows the weights, not the order', () => {
    expect(negotiateLocale('de;q=0.9,fr;q=0.5,en;q=0.8')).toBe('en');
  });

  it('keeps the first of two languages of the same weight', () => {
    expect(negotiateLocale('en,fr')).toBe('en');
  });

  it('skips a language the site does not speak', () => {
    expect(negotiateLocale('de,en;q=0.2')).toBe('en');
  });

  it('never gives a language the reader refused', () => {
    expect(negotiateLocale('en;q=0,fr;q=0.1')).toBe('fr');
  });

  it('falls back to French when the header is absent, empty or of no language the site speaks', () => {
    expect(negotiateLocale(null)).toBe('fr');
    expect(negotiateLocale('')).toBe('fr');
    expect(negotiateLocale('de,ja;q=0.5')).toBe('fr');
    expect(negotiateLocale('*')).toBe('fr');
  });
});
