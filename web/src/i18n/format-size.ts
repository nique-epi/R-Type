import type { Locale } from './locales';

const BYTES_PER_MEGABYTE = 1_000_000;
const BYTES_PER_GIGABYTE = 1_000_000_000;

function oneDecimalAtMost(size: number, locale: Locale): string {
  return new Intl.NumberFormat(locale, { maximumFractionDigits: 1 }).format(
    size,
  );
}

/** A size in megabytes of a million bytes, one decimal at most: "15,7" in French, "15.7" in English. */
export function formatMegabytes(bytes: number, locale: Locale): string {
  return oneDecimalAtMost(bytes / BYTES_PER_MEGABYTE, locale);
}

/** A size in gigabytes of a billion bytes, one decimal at most: "3,3" in French, "3.3" in English. */
export function formatGigabytes(bytes: number, locale: Locale): string {
  return oneDecimalAtMost(bytes / BYTES_PER_GIGABYTE, locale);
}
