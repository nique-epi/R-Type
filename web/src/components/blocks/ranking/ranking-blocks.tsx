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
  asksForAServer,
  type RankingSearch,
  rankingChoiceOf,
  rankingRowsOf,
} from '@/lib/rtype/ranking-view';

/**
 * Reads the servers the filters offer and the leaderboard of the choice in
 * the address. The two reads go out together unless the address asks for a
 * server: only then does the choice wait for the list.
 */
async function readRanking(search: RankingSearch) {
  if (asksForAServer(search)) {
    const servers = await getRankingServers();
    const choice = rankingChoiceOf(search, servers);
    return { servers, choice, ranking: await getRanking(choice) };
  }

  const choice = rankingChoiceOf(search, []);
  const [servers, ranking] = await Promise.all([
    getRankingServers(),
    getRanking(choice),
  ]);
  return { servers, choice, ranking };
}

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
  const { servers, choice, ranking } = await readRanking(search);

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
