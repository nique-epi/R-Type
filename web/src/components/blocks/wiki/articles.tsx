import type * as React from 'react';
import type { Locale } from '@/i18n/locales';
import { Accessibility } from './accessibility';
import { Enemies } from './enemies';
import { HostGuide } from './host-guide';
import { Keys } from './keys';

/** The articles of the wiki, in the order of its side menu. */
export const WIKI_ARTICLES = {
  keys: Keys,
  enemies: Enemies,
  accessibility: Accessibility,
  host: HostGuide,
} satisfies Record<string, (props: { lang: Locale }) => React.ReactNode>;

export type WikiSection = keyof typeof WIKI_ARTICLES;

export const WIKI_SECTIONS = Object.keys(WIKI_ARTICLES) as WikiSection[];

export function isWikiSection(value: string): value is WikiSection {
  return Object.hasOwn(WIKI_ARTICLES, value);
}
