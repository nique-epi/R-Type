'use client';

import { useRouter } from 'next/navigation';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import {
  type RankingChoice,
  type RankingServer,
  rankingChoiceAfter,
  rankingChoiceOfTab,
  rankingFiltersOf,
  rankingQueryOf,
  rankingSummaryOf,
  rankingTabOf,
} from '@/lib/rtype/ranking-view';
import { pathOf } from '@/lib/site-paths';
import {
  type RankingRow,
  RankingScreen,
  type RankingScreenLabels,
} from './ranking-screen';

export type RankingSectionProps = {
  locale: Locale;
  /** The servers the first tab can read. */
  servers: readonly RankingServer[];
  /** The scope and the filters in force, read from the address. */
  choice: RankingChoice;
  rows: readonly RankingRow[];
  copy: PageCopy['rankingPage'];
  labels: RankingScreenLabels;
  /** The master did not answer, so the list is empty for that reason. */
  unavailable: boolean;
};

/**
 * The leaderboard of the site: the screen of the game with the scope and the
 * filters of the address. A tab or a filter does not change what the screen
 * holds: it goes to the address that holds it, and the server reads the
 * master again for the page that comes back.
 */
export function RankingSection({
  locale,
  servers,
  choice,
  rows,
  copy,
  labels,
  unavailable,
}: RankingSectionProps) {
  const router = useRouter();
  const [serverTab = '', globalTab = ''] = copy.tabs;

  function goTo(next: RankingChoice) {
    router.push(`${pathOf(locale, 'ranking')}${rankingQueryOf(next)}`, {
      scroll: false,
    });
  }

  return (
    <RankingScreen
      labels={labels}
      tabs={[serverTab, globalTab]}
      tab={rankingTabOf(choice)}
      onTabChange={tab => goTo(rankingChoiceOfTab(choice, tab))}
      rows={rows}
      summary={rankingSummaryOf(choice, servers, copy, locale)}
      empty={unavailable ? copy.unavailable : copy.empty}
      filters={rankingFiltersOf(choice, servers, copy)}
      onApplyFilters={indices =>
        goTo(rankingChoiceAfter(choice, servers, indices))
      }
      onBack={() => router.push(pathOf(locale, 'home'))}
    />
  );
}
