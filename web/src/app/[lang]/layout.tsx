import type { Metadata } from 'next';
import { SiteShell } from '@/components/blocks/shell/site-shell';
import { getDictionary } from '@/i18n/get-dictionary';
import { isLocale, LOCALES } from '@/i18n/locales';
import { readLocale } from '@/i18n/read-locale';
import { FONT_VARIABLES } from '@/lib/fonts';
import { pathOf } from '@/lib/site-paths';
import '@/styles/globals.css';

export function generateStaticParams() {
  return LOCALES.map(lang => ({ lang }));
}

export async function generateMetadata({
  params,
}: LayoutProps<'/[lang]'>): Promise<Metadata> {
  const { lang } = await params;

  if (!isLocale(lang)) {
    return {};
  }

  return {
    title: { default: 'R-Type', template: '%s · R-Type' },
    description: getDictionary(lang).pages.site.metaDescription,
    alternates: {
      languages: Object.fromEntries(
        LOCALES.map(locale => [locale, pathOf(locale, 'home')]),
      ),
    },
  };
}

export default async function LocaleLayout({
  children,
  params,
}: LayoutProps<'/[lang]'>) {
  const lang = await readLocale(params);

  return (
    <html lang={lang} className={`${FONT_VARIABLES} h-full antialiased`}>
      <body className="flex min-h-full flex-col">
        <SiteShell lang={lang}>{children}</SiteShell>
      </body>
    </html>
  );
}
