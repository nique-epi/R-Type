import { DEFAULT_LOCALE, isLocale, type Locale } from './locales';

/** The language of an `Accept-Language` entry: "fr-CH;q=0.8" is "fr". */
function languageOf(entry: string): string {
  return (entry.split(';')[0] ?? '').trim().split('-')[0]?.toLowerCase() ?? '';
}

/** The weight an entry gives its language: 1 unless it says `q=`; 0 is "never". */
function weightOf(entry: string): number {
  const weight = /;\s*q=([\d.]+)/i.exec(entry)?.[1];
  const parsed = weight === undefined ? 1 : Number(weight);

  return Number.isNaN(parsed) ? 0 : parsed;
}

/**
 * The language of the site that an `Accept-Language` header prefers: the
 * heaviest entry the site speaks, the first of them on a tie, French when
 * the header names none (or is absent).
 */
export function negotiateLocale(acceptLanguage: string | null): Locale {
  const entries = (acceptLanguage ?? '')
    .split(',')
    .map(entry => ({ language: languageOf(entry), weight: weightOf(entry) }))
    .filter(entry => entry.weight > 0 && isLocale(entry.language));
  const best = entries.reduce<(typeof entries)[number] | undefined>(
    (kept, entry) => (kept && kept.weight >= entry.weight ? kept : entry),
    undefined,
  );

  return best && isLocale(best.language) ? best.language : DEFAULT_LOCALE;
}
