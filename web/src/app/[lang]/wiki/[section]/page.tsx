import type { Metadata } from 'next';
import { notFound } from 'next/navigation';
import { PageHero } from '@/components/layout/page-hero';
import { PageMain } from '@/components/primitives/layout/page-main';
import { WikiNav } from '@/components/wiki/wiki-nav';
import { getDictionary } from '@/i18n/get-dictionary';
import { LOCALES } from '@/i18n/locales';
import { readLocale } from '@/i18n/read-locale';
import { wikiPathOf } from '@/lib/site-paths';
import { isWikiSection, WIKI_ARTICLES, WIKI_SECTIONS } from './articles';

export function generateStaticParams() {
  return LOCALES.flatMap(lang =>
    WIKI_SECTIONS.map(section => ({ lang, section })),
  );
}

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/wiki/[section]'>): Promise<Metadata> {
  const lang = await readLocale(params);
  const { section } = await params;

  return isWikiSection(section)
    ? { title: getDictionary(lang).pages.wikiPage.sections[section] }
    : {};
}

export default async function WikiArticlePage({
  params,
}: PageProps<'/[lang]/wiki/[section]'>) {
  const lang = await readLocale(params);
  const { section } = await params;

  if (!isWikiSection(section)) {
    notFound();
  }

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
