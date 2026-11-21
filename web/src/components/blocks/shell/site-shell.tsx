import { AnnouncementBar } from '@/components/layout/announcement-bar';
import { PageHeader } from '@/components/layout/page-header';
import { SmoothScroll } from '@/components/motion/smooth-scroll';
import { getLatestRelease } from '@/hooks/api';
import { format } from '@/i18n/format';
import { formatDay } from '@/i18n/format-date';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { currentAccount } from '@/lib/session/current-account';
import { pathOf } from '@/lib/site-paths';
import { signOutAction } from '../sign-in/sign-out.action';

/**
 * What every page sits in: the announcement of the latest release, the
 * header with its menu, the page, and the smooth scroll of the
 * whole. The header and the menu offer to sign in or up to a guest, and
 * name the account of a visitor signed in, with a way to sign out.
 */
export async function SiteShell({
  lang,
  children,
}: {
  lang: Locale;
  children: React.ReactNode;
}) {
  const { pages, header } = getDictionary(lang);
  const [release, account] = await Promise.all([
    getLatestRelease(),
    currentAccount(),
  ]);
  const signInHref = pathOf(lang, 'signIn');
  const guestItems = account
    ? []
    : [
        { href: signInHref, label: pages.nav.signIn },
        { href: pathOf(lang, 'signUp'), label: pages.nav.signUp },
      ];
  const items = [
    { href: pathOf(lang, 'home'), label: pages.nav.home, exact: true },
    { href: pathOf(lang, 'download'), label: pages.nav.download },
    { href: pathOf(lang, 'servers'), label: pages.nav.servers },
    { href: pathOf(lang, 'ranking'), label: pages.nav.ranking },
    { href: pathOf(lang, 'wiki'), label: pages.nav.wiki },
    { href: pathOf(lang, 'team'), label: pages.nav.team },
    ...guestItems,
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
        account={account ? { nickname: account.nickname } : null}
        signInHref={signInHref}
        signOut={signOutAction}
      />
      <div className="flex-1">{children}</div>
    </SmoothScroll>
  );
}
