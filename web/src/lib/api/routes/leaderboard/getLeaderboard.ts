import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { LeaderboardDifficulty, LeaderboardRow } from '../../schemas';

/** Which leaderboard to read: one difficulty and one number of players, of one server or of all. */
export type LeaderboardQuery = {
  difficulty: LeaderboardDifficulty;
  playerCount: number;
  /** The id of a server; every server when left out. */
  server?: number;
  /** The best players read, by rank. */
  limit: number;
};

/** The best score of each player of a leaderboard, by rank, at most `limit` of them. */
export async function getLeaderboard(
  query: LeaderboardQuery,
): Promise<ApiResponse<LeaderboardRow[]>> {
  const { data, error } = await master('@get/leaderboard', { query });

  if (error) {
    return handleApiError<LeaderboardRow[]>(error);
  }

  return { data: data.scores, error: null };
}
