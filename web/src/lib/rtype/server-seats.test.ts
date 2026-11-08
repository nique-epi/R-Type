import { describe, expect, it } from 'vitest';

import { canJoin, freeSeatsOf } from './server-seats';
import type { MapServer } from './servers';

function server(overrides: Partial<MapServer>): MapServer {
  return {
    id: 'nebula',
    name: 'Nebula Lyon',
    status: 'online',
    version: 'v0.1.0',
    compatible: true,
    number: 3,
    ping: 18,
    capacity: 32,
    games: [],
    ...overrides,
  };
}

describe('canJoin', () => {
  const online = server({});

  it('lets a pilot into a waiting game with a free seat', () => {
    expect(canJoin(online, { pilots: 3, state: 'waiting' })).toBe(true);
  });

  it('refuses a game of four pilots, one in play, and any game of an offline server', () => {
    expect(canJoin(online, { pilots: 4, state: 'waiting' })).toBe(false);
    expect(canJoin(online, { pilots: 1, state: 'playing' })).toBe(false);
    expect(
      canJoin(server({ status: 'offline' }), { pilots: 1, state: 'waiting' }),
    ).toBe(false);
  });
});

describe('freeSeatsOf', () => {
  const games = [
    { pilots: 2, state: 'waiting' },
    { pilots: 3, state: 'waiting' },
    { pilots: 4, state: 'playing' },
    { pilots: 1, state: 'playing' },
  ] as const;

  it('counts the free seats of the games that wait, and none of a game in play', () => {
    expect(freeSeatsOf(server({ games }))).toBe(3);
  });

  it('counts none on a server that does not answer', () => {
    expect(freeSeatsOf(server({ games, status: 'offline' }))).toBe(0);
  });
});
