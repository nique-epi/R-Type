import type { Metadata } from 'next';
import { notFound } from 'next/navigation';
import {
  isWikiSection,
  WIKI_SECTIONS,
} from '@/components/blocks/wiki/articles';
import { WikiBlocks } from '@/components/blocks/wiki/wiki-blocks';
import { getDictionary } from '@/i18n/get-dictionary';
import { LOCALES } from '@/i18n/locales';
import { readLocale } from '@/i18n/read-locale';

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

  return <WikiBlocks lang={lang} section={section} />;
}
