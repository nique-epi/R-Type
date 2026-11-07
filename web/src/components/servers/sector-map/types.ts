import type { CountTemplates } from '@/i18n/format';
import type { GameState, ServerStatus } from '@/lib/rtype/servers';

/** A game of a server, as the map lists it. */
export type MapGame = {
  pilots: number;
  state: GameState;
};

/** A server on the map. */
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

/** The seat the reader holds, if any. */
export type HeldSeat = {
  serverId: string;
  gameIndex: number;
  seat: number;
};

/** How far the map is zoomed: the whole sky, one server, or the games of that server. */
export type MapStage = 'whole' | 'close' | 'games';

/**
 * The words of the map, French or English. Pixel lines are written without
 * accents (Press Start 2P has no glyph for them); `{…}` holes are filled by
 * the map.
 */
export type SectorMapLabels = {
  screen: string;
  title: string;
  servers: CountTemplates;
  subtitle: string;
  ring: string;
  star: string;
  starChosen: string;
  bubblePilots: string;
  bubblePing: string;
  bubbleSlow: string;
  bubbleZoom: string;
  bubbleGames: string;
  gamesOf: string;
  columnGame: string;
  columnPilots: string;
  columnState: string;
  game: string;
  gameRow: string;
  states: Record<GameState, string>;
  noGames: string;
  youAreIn: string;
  pressEnterToJoin: string;
  leaveToJoin: string;
  inProgress: string;
  full: string;
  quickPlay: string;
  join: string;
  leave: string;
  sheet: string;
  back: string;
  leaderboard: string;
  host: string;
  keys: {
    quickPlay: string;
    join: string;
    leave: string;
    sheet: string;
    back: string;
    leaderboard: string;
    host: string;
  };
};
