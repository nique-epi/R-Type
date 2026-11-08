import type { Metadata } from 'next';
import { connection } from 'next/server';
import { RankingBlocks } from '@/components/blocks/ranking/ranking-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/leaderboard'>): Promise<Metadata> {
  return titleMetadata(params, 'rankingPage');
}

export default async function LeaderboardPage({
  params,
  searchParams,
}: PageProps<'/[lang]/leaderboard'>) {
  await connection();
  return (
    <RankingBlocks
      lang={await readLocale(params)}
      search={await searchParams}
    />
  );
}
