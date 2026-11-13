import type { CountTemplates } from '@/i18n/format';
import type { GameState } from '@/lib/rtype/servers';

export type { MapGame, MapServer } from '@/lib/rtype/servers';

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
