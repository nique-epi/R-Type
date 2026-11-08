import {
  AutoRefresh,
  LEADERBOARD_REFRESH_SECONDS,
} from '@/components/layout/auto-refresh';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { RankingSection } from '@/components/ranking/ranking-section';
import { StarLayers } from '@/components/sky/star-layers';
import { getRanking, getRankingServers } from '@/hooks/api';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import {
  type RankingSearch,
  rankingChoiceOf,
  rankingRowsOf,
} from '@/lib/rtype/ranking-view';

/**
 * The leaderboard page under its hero, on a sky of drifting stars that fills
 * the whole page: the screen of the game with the scores the master keeps
 * for the scope and the filters the address holds.
 */
export async function RankingBlocks({
  lang,
  search,
}: {
  lang: Locale;
  search: RankingSearch;
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.rankingPage;
  const servers = await getRankingServers();
  const choice = rankingChoiceOf(search, servers);
  const ranking = await getRanking(choice);

  return (
    <div className="relative isolate">
      <AutoRefresh seconds={LEADERBOARD_REFRESH_SECONDS} />
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody>
        <RankingSection
          locale={lang}
          servers={servers}
          choice={choice}
          rows={rankingRowsOf(ranking.players, copy)}
          copy={copy}
          labels={dictionary.ranking}
          unavailable={!ranking.available}
        />
      </PageBody>
    </div>
  );
}
