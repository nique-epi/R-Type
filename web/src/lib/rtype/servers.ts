/** The status the master keeps for a game server. */
export type ServerStatus = 'online' | 'degraded' | 'offline';

/** Where a game stands, as the server list shows it. */
export type GameState = 'waiting' | 'playing' | 'full';

/** A game of a server: the pilots in it and where it stands. */
export type MapGame = {
  pilots: number;
  state: GameState;
};

/** A server as the pages and the sector map know it. */
export type MapServer = {
  id: string;
  name: string;
  status: ServerStatus;
  /** "v0.1.0". */
  version: string;
  /** Whether its version matches the client's; an incompatible server is drawn hollow. */
  compatible: boolean;
  /** The number the master gave it; its bearing is drawn from it. */
  number: number;
  /** The round trip to it, in milliseconds: its distance from the planet. */
  ping: number;
  /** The most pilots it takes, in all its games. */
  capacity: number;
  games: readonly MapGame[];
};
