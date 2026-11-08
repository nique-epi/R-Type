import { MAXIMUM_PILOTS_PER_GAME } from './players';
import type { MapGame, MapServer } from './servers';

/** The pilots of a server, in all its games. */
export function pilotsOf(server: MapServer): number {
  return server.games.reduce((sum, game) => sum + game.pilots, 0);
}

/** A game can be joined when its server answers, it waits for its pilots and a seat is free. */
export function canJoin(server: MapServer, game: MapGame): boolean {
  return (
    server.status !== 'offline' &&
    game.state === 'waiting' &&
    game.pilots < MAXIMUM_PILOTS_PER_GAME
  );
}

/** The seats open to a pilot in all the games of a server that answers. */
export function freeSeatsOf(server: MapServer): number {
  return server.games
    .filter(game => canJoin(server, game))
    .reduce((sum, game) => sum + MAXIMUM_PILOTS_PER_GAME - game.pilots, 0);
}
