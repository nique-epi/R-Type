import { en } from './dictionaries/en';
import { fr } from './dictionaries/fr';
import type { Dictionary } from './dictionary';
import type { Locale } from './locales';

const DICTIONARIES: Record<Locale, Dictionary> = { fr, en };

/** The fixed words of the components in a language. */
export function getDictionary(locale: Locale): Dictionary {
  return DICTIONARIES[locale];
}
