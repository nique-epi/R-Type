import { describe, expect, it } from 'vitest';

import type { ListedServer } from '@/lib/api/schemas';
import { mapServerOf, UNMEASURED_PING_MILLISECONDS } from './map-server';
import { sampleSeries } from './sample-servers';

const LISTED: ListedServer = {
  id: 7,
  name: 'Paris 1',
  gameVersion: '0.1.0',
  maximumGames: 8,
  maximumPlayersPerGame: 4,
  status: 'online',
  gameCount: 2,
  playerCount: 3,
};

describe('mapServerOf', () => {
  it('draws a server by the number the master gave it', () => {
    const server = mapServerOf(LISTED, { games: [], ping: 30 }, '0.1.0');

    expect(server.id).toBe('7');
    expect(server.number).toBe(7);
    expect(server.name).toBe('Paris 1');
  });

  it('counts the seats of every game it can host', () => {
    expect(mapServerOf(LISTED, { games: [], ping: 30 }, '0.1.0').capacity).toBe(
      32,
    );
  });

  it('folds the game states of the master to the ones of the site', () => {
    const server = mapServerOf(
      LISTED,
      {
        games: [
          { number: 1, name: 'GAME 1', state: 'waiting', playerCount: 1 },
          { number: 2, name: 'GAME 2', state: 'inGame', playerCount: 4 },
          { number: 3, name: 'GAME 3', state: 'full', playerCount: 4 },
        ],
        ping: 30,
      },
      '0.1.0',
    );

    expect(server.games).toEqual([
      { pilots: 1, state: 'waiting' },
      { pilots: 4, state: 'playing' },
      { pilots: 4, state: 'full' },
    ]);
  });

  it.each([
    ['unreachable', 'offline'],
    ['maintenance', 'offline'],
    ['offline', 'offline'],
    ['degraded', 'degraded'],
    ['online', 'online'],
  ] as const)('draws a %s server as %s', (status, drawn) => {
    expect(
      mapServerOf({ ...LISTED, status }, { games: [], ping: 30 }, '0.1.0')
        .status,
    ).toBe(drawn);
  });

  it('gives a server with no measure a distance of its own', () => {
    expect(
      mapServerOf(LISTED, { games: [], ping: undefined }, '0.1.0').ping,
    ).toBe(UNMEASURED_PING_MILLISECONDS);
  });

  it('marks a server of another version of the game as incompatible', () => {
    const other = { ...LISTED, gameVersion: '0.0.9' };

    expect(
      mapServerOf(other, { games: [], ping: 30 }, '0.1.0').compatible,
    ).toBe(false);
    expect(
      mapServerOf(other, { games: [], ping: 30 }, undefined).compatible,
    ).toBe(true);
  });
});

describe('sampleSeries', () => {
  it('gives the same counts for the same seed', () => {
    expect(sampleSeries(7, 48, 12)).toEqual(sampleSeries(7, 48, 12));
  });

  it('keeps every count between 0 and the maximum', () => {
    for (const value of sampleSeries(21, 200, 12)) {
      expect(value).toBeGreaterThanOrEqual(0);
      expect(value).toBeLessThanOrEqual(12);
    }
  });
});
