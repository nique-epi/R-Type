import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { getLeaderboard } from './getLeaderboard';

const ROW = {
  rank: 1,
  nickname: 'ACE',
  score: 12000,
  playedAt: 1800000000,
  serverId: 7,
  serverName: 'Paris 1',
};

describe('getLeaderboard', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the leaderboard of the difficulty and the players it is given', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { scores: [] }));
    vi.stubGlobal('fetch', fetch);

    await getLeaderboard({ difficulty: 'hard', playerCount: 3, limit: 10 });

    const url = new URL(String(fetch.mock.calls[0]?.[0]));
    expect(url.pathname).toBe('/api/leaderboard');
    expect(Object.fromEntries(url.searchParams)).toEqual({
      difficulty: 'hard',
      playerCount: '3',
      limit: '10',
    });
  });

  it('asks one server only when it is given one', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { scores: [] }));
    vi.stubGlobal('fetch', fetch);

    await getLeaderboard({
      difficulty: 'normal',
      playerCount: 2,
      server: 7,
      limit: 10,
    });

    const url = new URL(String(fetch.mock.calls[0]?.[0]));
    expect(url.searchParams.get('server')).toBe('7');
  });

  it('returns the lines of the answer, in the order of the master', async () => {
    const scores = [ROW, { ...ROW, rank: 2, nickname: 'R9', score: 800 }];
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue(answer(200, { scores })));

    expect(
      await getLeaderboard({ difficulty: 'normal', playerCount: 2, limit: 10 }),
    ).toEqual({ data: scores, error: null });
  });

  it('maps a request the master refuses to BAD_REQUEST', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(answer(400, { status: 400, title: 'Bad Request' })),
    );

    expect(
      (
        await getLeaderboard({
          difficulty: 'normal',
          playerCount: 2,
          limit: 10,
        })
      ).error,
    ).toBe('BAD_REQUEST');
  });

  it('rejects a line whose rank is not a number', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(
          answer(200, { scores: [{ ...ROW, rank: 'first' }] }),
        ),
    );

    expect(
      (
        await getLeaderboard({
          difficulty: 'normal',
          playerCount: 2,
          limit: 10,
        })
      ).error,
    ).toBe('ERR_API_VALIDATION_FAILED');
  });
});
