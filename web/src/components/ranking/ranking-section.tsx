'use client';

import { useRouter } from 'next/navigation';
import { useState } from 'react';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import {
  FIRST_RANKING_CHOICE,
  rankingChoiceAfter,
  rankingFiltersOf,
  rankingRowsOf,
  rankingSummaryOf,
} from '@/lib/rtype/ranking-view';
import { pathOf } from '@/lib/site-paths';
import { RankingScreen, type RankingScreenLabels } from './ranking-screen';

export type RankingSectionProps = {
  locale: Locale;
  /** The names of the servers the first tab can read. */
  servers: readonly string[];
  copy: PageCopy['rankingPage'];
  labels: RankingScreenLabels;
};

/**
 * The leaderboard of the site: the screen of the game with the scope and the
 * filters the reader chose. The scores are examples, the master keeps none
 * yet, and the screen says so under the list.
 */
export function RankingSection({
  locale,
  servers,
  copy,
  labels,
}: RankingSectionProps) {
  const router = useRouter();
  const [choice, setChoice] = useState(FIRST_RANKING_CHOICE);
  const [serverTab = '', globalTab = ''] = copy.tabs;

  return (
    <RankingScreen
      labels={labels}
      tabs={[serverTab, globalTab]}
      tab={choice.tab}
      onTabChange={tab => setChoice({ ...choice, tab })}
      rows={rankingRowsOf(choice, copy)}
      summary={rankingSummaryOf(choice, servers, copy, locale)}
      footnote={copy.footnote}
      filters={rankingFiltersOf(choice, servers, copy)}
      onApplyFilters={indices =>
        setChoice(rankingChoiceAfter(choice, servers, indices))
      }
      onBack={() => router.push(pathOf(locale, 'home'))}
    />
  );
}
