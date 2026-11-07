import type { Locale } from './locales';

/** A day written out, as the announcement of a release gives it: "6 novembre 2026". */
export function formatDay(isoDate: string, locale: Locale): string {
  return new Intl.DateTimeFormat(locale, {
    day: 'numeric',
    month: 'long',
    year: 'numeric',
    timeZone: 'UTC',
  }).format(new Date(isoDate));
}
