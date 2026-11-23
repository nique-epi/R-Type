import { z } from 'zod';

/** One line of a leaderboard: the best score of a player, and where and when it was made. */
export const leaderboardRowSchema = z.object({
  rank: z.number().int(),
  nickname: z.string(),
  score: z.number().int(),
  /** Seconds since 1970 UTC. */
  playedAt: z.number().int(),
  serverId: z.number().int(),
  serverName: z.string(),
});

export type LeaderboardRow = z.infer<typeof leaderboardRowSchema>;

export const leaderboardSchema = z.object({
  scores: z.array(leaderboardRowSchema),
  /** The line of the player the request named, wherever it ranks. */
  player: leaderboardRowSchema.optional(),
});

/** One line of the leaderboard of the accounts: the total of the scores of an account. */
export const accountLeaderboardRowSchema = z.object({
  rank: z.number().int(),
  /** The nickname of the account, whatever nickname it played under. */
  nickname: z.string(),
  /** The sum of the scores of every game credited to the account. */
  score: z.number().int(),
  /** When its last game ended, in seconds since 1970 UTC. */
  lastPlayedAt: z.number().int(),
});

export type AccountLeaderboardRow = z.infer<typeof accountLeaderboardRowSchema>;

export const accountLeaderboardSchema = z.object({
  accounts: z.array(accountLeaderboardRowSchema),
});
