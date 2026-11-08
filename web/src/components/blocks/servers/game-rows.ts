import type { Seat } from '@/components/game/player-slots';
import type { GameRow } from '@/components/servers/games-table';
import { format } from '@/i18n/format';
import type { PageCopy } from '@/i18n/page-copy';
import { MAXIMUM_PILOTS_PER_GAME, PLAYER_NUMBERS } from '@/lib/rtype/players';
import type { GameState, MapServer } from '@/lib/rtype/servers';

/** The four seats of a game, the first `pilots` of them taken. */
export function seatsOf(pilots: number, pilotLabel: string): Seat[] {
  return PLAYER_NUMBERS.map(player =>
    player <= pilots
      ? { player, title: format(pilotLabel, { number: player }) }
      : null,
  );
}

/** The games of a server as the rows of its table: read only, with no way to join. */
export function gameRowsOf(
  server: MapServer,
  copy: PageCopy['serverPage'],
  stateLabels: Record<GameState, string>,
): GameRow[] {
  return server.games.map((game, index) => ({
    id: `game-${index}`,
    label: format(copy.gameName, { number: index + 1 }),
    state: game.state,
    stateLabel: stateLabels[game.state],
    seats: seatsOf(
      Math.min(game.pilots, MAXIMUM_PILOTS_PER_GAME),
      copy.pilotCount,
    ),
  }));
}
