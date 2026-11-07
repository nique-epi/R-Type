import type { Metadata } from 'next';
import { AnnouncementBar } from '@/components/layout/announcement-bar';
import { PageHeader } from '@/components/layout/page-header';
import { SiteFooter } from '@/components/layout/site-footer';
import { SmoothScroll } from '@/components/motion/smooth-scroll';
import { getLatestRelease } from '@/hooks/api';
import { format } from '@/i18n/format';
import { formatDay } from '@/i18n/format-date';
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
  const { pages, header, footer } = getDictionary(lang);
  const release = await getLatestRelease();
  const items = [
    { href: pathOf(lang, 'home'), label: pages.nav.home, exact: true },
    { href: pathOf(lang, 'download'), label: pages.nav.download },
    { href: pathOf(lang, 'servers'), label: pages.nav.servers },
    { href: pathOf(lang, 'ranking'), label: pages.nav.ranking },
    { href: pathOf(lang, 'wiki'), label: pages.nav.wiki },
    { href: pathOf(lang, 'team'), label: pages.nav.team },
  ];

  return (
    <html lang={lang} className={`${FONT_VARIABLES} h-full antialiased`}>
      <body className="flex min-h-full flex-col">
        <SmoothScroll>
          <AnnouncementBar
            action={{
              href: pathOf(lang, 'download'),
              label: pages.site.releaseNotes,
            }}
          >
            {release
              ? format(pages.site.announcement, {
                  version: release.version,
                  date: formatDay(release.publishedAt, lang),
                })
              : pages.site.announcementFallback}
          </AnnouncementBar>
          <PageHeader
            homeHref={pathOf(lang, 'home')}
            items={items}
            downloadHref={pathOf(lang, 'download')}
            menuFootnote={
              release
                ? format(pages.site.menuFootnote, { version: release.version })
                : pages.site.menuFootnoteFallback
            }
            labels={header}
          />
          <div className="flex-1">{children}</div>
          <SiteFooter
            aria-label={footer.navigation}
            links={[
              { href: pathOf(lang, 'download'), label: pages.nav.download },
              { href: pathOf(lang, 'servers'), label: pages.nav.servers },
              { href: pathOf(lang, 'wiki'), label: pages.nav.wiki },
              {
                href: pathOf(lang, 'hostServer'),
                label: pages.site.hostServer,
              },
            ]}
          >
            {release
              ? format(pages.site.footer, { version: release.version })
              : pages.site.footerFallback}
          </SiteFooter>
        </SmoothScroll>
      </body>
    </html>
  );
}
