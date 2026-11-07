/** The status the master keeps for a game server. */
export type ServerStatus = 'online' | 'degraded' | 'offline';

/** Where a game stands, as the server list shows it. */
export type GameState = 'waiting' | 'playing' | 'full';
