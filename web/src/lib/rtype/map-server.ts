import type { ListedServer, MasterGame, MasterStatus } from '@/lib/api/schemas';
import type { GameState, MapServer, ServerStatus } from './servers';

/**
 * The round trip, in milliseconds, of a server that took no measure of it:
 * a server with no player has no ping, and the map still needs a distance.
 */
export const UNMEASURED_PING_MILLISECONDS = 75;

/**
 * The statuses of the master folded to the three the site draws. A server
 * whose UDP port cannot be reached, or an admin took out of service, cannot
 * be played on: it is drawn offline.
 */
const STATUS_OF_MASTER: Record<MasterStatus, ServerStatus> = {
  online: 'online',
  degraded: 'degraded',
  unreachable: 'offline',
  offline: 'offline',
  maintenance: 'offline',
};

const GAME_STATE_OF_MASTER: Record<MasterGame['state'], GameState> = {
  waiting: 'waiting',
  full: 'full',
  inGame: 'playing',
};

/** What the map knows of a server beyond the line the master lists it by. */
export type ServerDetails = {
  games: readonly MasterGame[];
  /** The mean round trip of its players at its last sample, when it took one. */
  ping: number | undefined;
};

/** A server of the master as the map draws it. */
export function mapServerOf(
  listed: ListedServer,
  details: ServerDetails,
  latestVersion: string | undefined,
): MapServer {
  return {
    id: String(listed.id),
    name: listed.name,
    status: STATUS_OF_MASTER[listed.status],
    version: `v${listed.gameVersion}`,
    compatible:
      latestVersion === undefined || listed.gameVersion === latestVersion,
    number: listed.id,
    ping: details.ping ?? UNMEASURED_PING_MILLISECONDS,
    capacity: listed.maximumGames * listed.maximumPlayersPerGame,
    games: details.games.map(game => ({
      pilots: game.playerCount,
      state: GAME_STATE_OF_MASTER[game.state],
    })),
  };
}
