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
