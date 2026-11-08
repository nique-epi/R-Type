import { describe, expect, it } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import type { MapServer } from '@/lib/rtype/servers';
import { gameRowsOf, seatsOf } from './game-rows';

const SERVER: MapServer = {
  id: '7',
  name: 'Paris 1',
  status: 'online',
  version: 'v0.1.0',
  compatible: true,
  number: 7,
  ping: 20,
  capacity: 32,
  games: [
    { pilots: 2, state: 'waiting' },
    { pilots: 4, state: 'playing' },
  ],
};

describe('seatsOf', () => {
  it('takes the first seats and leaves the others free, always four', () => {
    const seats = seatsOf(2, 'Pilote {number}');

    expect(seats).toHaveLength(4);
    expect(seats.map(seat => seat?.player ?? null)).toEqual([1, 2, null, null]);
    expect(seats[1]?.title).toBe('Pilote 2');
  });

  it('fills no seat for a game with no pilot', () => {
    expect(seatsOf(0, 'Pilote {number}').every(seat => seat === null)).toBe(
      true,
    );
  });
});

describe('gameRowsOf', () => {
  const rows = gameRowsOf(SERVER, fr.pages.serverPage, fr.gameState);

  it('numbers the games from 1 and gives each its state in words', () => {
    expect(rows.map(row => row.label)).toEqual(['Partie 1', 'Partie 2']);
    expect(rows.map(row => row.stateLabel)).toEqual(['En attente', 'En jeu']);
  });

  it('draws the pilots of each game in its seats', () => {
    expect(rows[1]?.seats.every(seat => seat !== null)).toBe(true);
  });
});
