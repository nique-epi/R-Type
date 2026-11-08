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
 * heaviest entry the site speaks, the first of them on a tie (the sort is
 * stable), French when the header names none (or is absent).
 */
export function negotiateLocale(acceptLanguage: string | null): Locale {
  const spoken = (acceptLanguage ?? '').split(',').flatMap(entry => {
    const language = languageOf(entry);
    const weight = weightOf(entry);

    return weight > 0 && isLocale(language) ? [{ language, weight }] : [];
  });

  return (
    spoken.sort((first, second) => second.weight - first.weight)[0]?.language ??
    DEFAULT_LOCALE
  );
}
