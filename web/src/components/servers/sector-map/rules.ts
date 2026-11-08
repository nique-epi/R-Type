import { canJoin, pilotsOf } from '@/lib/rtype/server-seats';
import type { MapServer } from '@/lib/rtype/servers';

/** A server with more pilots than this is drawn as a crowd. */
const CROWD_PILOTS = 10;

/** The mark of a star: a cross, a large cross, a dot, or a hollow square. */
export type StarMark = 'cross' | 'crowd' | 'empty' | 'off';

/** The color of a star, by the token it is drawn in. */
export type StarTone = 'lit' | 'slow' | 'dim';

/** How a star is drawn: hollow when its version differs, a crowd past ten pilots. */
export function starLook(server: MapServer): {
  mark: StarMark;
  tone: StarTone;
} {
  const pilots = pilotsOf(server);
  let mark: StarMark = 'empty';
  if (!server.compatible) {
    mark = 'off';
  } else if (pilots > CROWD_PILOTS) {
    mark = 'crowd';
  } else if (pilots > 0) {
    mark = 'cross';
  }
  let tone: StarTone = 'lit';
  if (!server.compatible || pilots === 0) {
    tone = 'dim';
  } else if (server.status === 'degraded') {
    tone = 'slow';
  }
  return { mark, tone };
}

/** The servers the map draws, nearest first: an offline server has no star. */
export function serversOnMap(servers: readonly MapServer[]): MapServer[] {
  return servers
    .filter(server => server.status !== 'offline')
    .sort((first, second) => first.ping - second.ping);
}

/** The server `step` places after the chosen one, nearest first, wrapping around. */
export function cycleServer(
  servers: readonly MapServer[],
  chosenId: string | null,
  step: number,
): MapServer | undefined {
  if (servers.length === 0) {
    return undefined;
  }
  const index = servers.findIndex(server => server.id === chosenId);
  return servers[(index + step + servers.length) % servers.length];
}

/** The nearest server with a game to join, and that game, for a quick play. */
export function quickPlayTarget(
  servers: readonly MapServer[],
): { server: MapServer; gameIndex: number } | undefined {
  for (const server of serversOnMap(servers)) {
    const gameIndex = server.games.findIndex(game => canJoin(server, game));
    if (gameIndex >= 0) {
      return { server, gameIndex };
    }
  }
  return undefined;
}
