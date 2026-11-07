import type * as React from 'react';
import type { Locale } from '@/i18n/locales';
import { Accessibility } from './_articles/accessibility';
import { Enemies } from './_articles/enemies';
import { HostGuide } from './_articles/host-guide';
import { Keys } from './_articles/keys';

/** The articles of the wiki, in the order of its side menu. */
export const WIKI_ARTICLES = {
  keys: Keys,
  enemies: Enemies,
  accessibility: Accessibility,
  host: HostGuide,
} satisfies Record<string, (props: { lang: Locale }) => React.ReactNode>;

type WikiSection = keyof typeof WIKI_ARTICLES;

export const WIKI_SECTIONS = Object.keys(WIKI_ARTICLES) as WikiSection[];

export function isWikiSection(value: string): value is WikiSection {
  return Object.hasOwn(WIKI_ARTICLES, value);
}
