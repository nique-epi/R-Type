import type { Locale } from './locales';

/**
 * A day written out, as the announcement of a release gives it ("6 novembre
 * 2026"), or with a short month for a list ("6 nov. 2026").
 */
export function formatDay(
  isoDate: string,
  locale: Locale,
  month: 'long' | 'short' = 'long',
): string {
  return new Intl.DateTimeFormat(locale, {
    day: 'numeric',
    month,
    year: 'numeric',
    timeZone: 'UTC',
  }).format(new Date(isoDate));
}
