/**
 * The difficulties a leaderboard is kept for, easiest first; two of them
 * never mix (docs "The global leaderboard"). Written apart from the schemas
 * so a component of the browser can name them without loading zod.
 */
export const LEADERBOARD_DIFFICULTIES = ['easy', 'normal', 'hard'] as const;

export type LeaderboardDifficulty = (typeof LEADERBOARD_DIFFICULTIES)[number];

/** The games of a leaderboard have from one to this many players (`MAXIMUM_PLAYERS` of the game). */
export const MAXIMUM_LEADERBOARD_PLAYERS = 4;
