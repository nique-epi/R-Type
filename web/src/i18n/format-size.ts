import type { Locale } from './locales';

const BYTES_PER_MEGABYTE = 1_000_000;

/** A size in megabytes of a million bytes, one decimal at most: "15,7" in French, "15.7" in English. */
export function formatMegabytes(bytes: number, locale: Locale): string {
  return new Intl.NumberFormat(locale, { maximumFractionDigits: 1 }).format(
    bytes / BYTES_PER_MEGABYTE,
  );
}
