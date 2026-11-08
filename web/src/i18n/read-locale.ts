import { notFound } from 'next/navigation';

import { isLocale, type Locale } from './locales';

/** The language a page is asked in, read from its `params`; a path in a language the site lacks is a 404. */
export async function readLocale(
  params: Promise<{ lang: string }>,
): Promise<Locale> {
  const { lang } = await params;

  if (!isLocale(lang)) {
    notFound();
  }

  return lang;
}
