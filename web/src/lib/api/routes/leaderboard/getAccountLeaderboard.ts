import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { AccountLeaderboardRow } from '../../schemas';

/** Which page of the leaderboard of the accounts to read. */
export type AccountLeaderboardQuery = {
  /** The best accounts read, by rank. */
  limit: number;
};

/**
 * The total of the scores of each account, by rank, at most `limit` of them:
 * every difficulty, number of players and server together.
 */
export async function getAccountLeaderboard(
  query: AccountLeaderboardQuery,
): Promise<ApiResponse<AccountLeaderboardRow[]>> {
  const { data, error } = await master('@get/leaderboard/accounts', { query });

  if (error) {
    return handleApiError<AccountLeaderboardRow[]>(error);
  }

  return { data: data.accounts, error: null };
}
