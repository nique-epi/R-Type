import type { Metadata } from 'next';
import { connection } from 'next/server';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { RankingSection } from '@/components/ranking/ranking-section';
import { getServerNames } from '@/hooks/api';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/leaderboard'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.rankingPage.title };
}

export default async function LeaderboardPage({
  params,
}: PageProps<'/[lang]/leaderboard'>) {
  await connection();
  const lang = await readLocale(params);
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.rankingPage;
  const servers = await getServerNames();

  return (
    <>
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody>
        <RankingSection
          locale={lang}
          servers={servers}
          copy={copy}
          labels={dictionary.ranking}
        />
      </PageBody>
    </>
  );
}
