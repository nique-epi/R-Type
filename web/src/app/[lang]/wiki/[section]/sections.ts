/** The articles of the wiki, in the order of its side menu. */
export const WIKI_SECTIONS = [
  'keys',
  'enemies',
  'accessibility',
  'host',
] as const;

export type WikiSection = (typeof WIKI_SECTIONS)[number];

export function isWikiSection(value: string): value is WikiSection {
  return (WIKI_SECTIONS as readonly string[]).includes(value);
}
