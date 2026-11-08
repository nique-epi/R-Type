import 'server-only';

import { api } from '@/lib/api';
import {
  RANKING_LENGTH,
  type RankedPlayer,
  type RankingChoice,
} from '@/lib/rtype/ranking-view';

/** The best players of a leaderboard; `available` is false when the master did not answer. */
export type Ranking = {
  available: boolean;
  players: readonly RankedPlayer[];
};

/**
 * Reads the leaderboard a choice names: one difficulty and one number of
 * players, of one server or of all. A master that does not answer gives no
 * player and says so, so the page can tell it from a leaderboard nobody has
 * played yet.
 */
export async function getRanking(choice: RankingChoice): Promise<Ranking> {
  const { data, error } = await api.leaderboard.getLeaderboard({
    difficulty: choice.difficulty,
    playerCount: choice.pilots,
    ...(choice.serverId === null ? {} : { server: choice.serverId }),
    limit: RANKING_LENGTH,
  });

  return error
    ? { available: false, players: [] }
    : { available: true, players: data };
}
