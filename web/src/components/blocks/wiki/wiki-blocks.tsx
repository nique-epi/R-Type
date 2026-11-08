import { PageHero } from '@/components/layout/page-hero';
import { PageMain } from '@/components/primitives/layout/page-main';
import { WikiNav } from '@/components/wiki/wiki-nav';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { wikiPathOf } from '@/lib/site-paths';
import { WIKI_ARTICLES, WIKI_SECTIONS, type WikiSection } from './articles';

/** A page of the wiki: its hero, the menu of its articles, and the article of `section`. */
export function WikiBlocks({
  lang,
  section,
}: {
  lang: Locale;
  section: WikiSection;
}) {
  const Article = WIKI_ARTICLES[section];
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.wikiPage;

  return (
    <>
      <PageHero eyebrow={copy.eyebrow} title={copy.title} />
      <PageMain className="flex flex-wrap items-start gap-12">
        <WikiNav
          aria-label={dictionary.wiki.navigation}
          current={wikiPathOf(lang, section)}
          items={WIKI_SECTIONS.map(id => ({
            href: wikiPathOf(lang, id),
            label: copy.sections[id],
          }))}
        />
        <article className="grid min-w-0 flex-[999_1_560px] gap-8">
          <Article lang={lang} />
        </article>
      </PageMain>
    </>
  );
}
