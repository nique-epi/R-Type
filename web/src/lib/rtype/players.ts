/** The seats of a game, as the server numbers its pilots. */
export const PLAYER_NUMBERS = [1, 2, 3, 4] as const;

export type PlayerNumber = (typeof PLAYER_NUMBERS)[number];

/** The most pilots a game takes, MAXIMUM_PLAYERS on the server. */
export const MAXIMUM_PILOTS_PER_GAME = PLAYER_NUMBERS.length;
