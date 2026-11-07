import type { Metadata } from 'next';
import { connection } from 'next/server';
import { RankingBlocks } from '@/components/blocks/ranking/ranking-blocks';
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
  return <RankingBlocks lang={await readLocale(params)} />;
}
