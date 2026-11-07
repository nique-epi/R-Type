import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from './answer';
import { getGames } from './getGames';

describe('getGames', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the games of the server it is given', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { games: [] }));
    vi.stubGlobal('fetch', fetch);

    await getGames(7);

    expect(String(fetch.mock.calls[0]?.[0])).toBe(
      'http://master.test:8080/api/servers/7/games',
    );
  });

  it('returns the games of the answer', async () => {
    const games = [
      { number: 1, name: 'GAME 1', state: 'waiting', playerCount: 1 },
      { number: 2, name: 'GAME 2', state: 'inGame', playerCount: 4 },
    ];
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue(answer(200, { games })));

    expect(await getGames(7)).toEqual({ data: games, error: null });
  });

  it('maps a server the master does not have to NOT_FOUND', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(answer(404, { status: 404, title: 'Not Found' })),
    );

    expect((await getGames(99)).error).toBe('NOT_FOUND');
  });

  it('rejects a game in a state the master does not fold to', async () => {
    const games = [
      { number: 1, name: 'GAME 1', state: 'playing', playerCount: 1 },
    ];
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue(answer(200, { games })));

    expect((await getGames(7)).error).toBe('ERR_API_VALIDATION_FAILED');
  });
});
