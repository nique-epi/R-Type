import type { Locale } from '@/i18n/locales';

/** The pages of the site, as the path under the language of each. */
const PAGE_PATHS = {
  home: '',
  download: '/download',
  servers: '/servers',
  hostServer: '/servers/new',
  ranking: '/leaderboard',
  wiki: '/wiki',
  team: '/team',
} as const;

export type SitePage = keyof typeof PAGE_PATHS;

/** The path of a page in a language: `/fr/servers`; the home page is `/fr`. */
export function pathOf(locale: Locale, page: SitePage): string {
  return `/${locale}${PAGE_PATHS[page]}`;
}

/** The path of one server: `/fr/servers/7`. */
export function serverPathOf(locale: Locale, serverId: string): string {
  return `${pathOf(locale, 'servers')}/${encodeURIComponent(serverId)}`;
}

/** The path of one section of the wiki: `/fr/wiki/keys`. */
export function wikiPathOf(locale: Locale, section: string): string {
  return `${pathOf(locale, 'wiki')}/${section}`;
}
