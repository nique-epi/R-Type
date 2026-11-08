import { AnnouncementBar } from '@/components/layout/announcement-bar';
import { PageHeader } from '@/components/layout/page-header';
import { SmoothScroll } from '@/components/motion/smooth-scroll';
import { getLatestRelease } from '@/hooks/api';
import { format } from '@/i18n/format';
import { formatDay } from '@/i18n/format-date';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { pathOf } from '@/lib/site-paths';

/**
 * What every page sits in: the announcement of the latest release, the
 * header with its menu, the page, and the smooth scroll of the
 * whole.
 */
export async function SiteShell({
  lang,
  children,
}: {
  lang: Locale;
  children: React.ReactNode;
}) {
  const { pages, header } = getDictionary(lang);
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
    </SmoothScroll>
  );
}
