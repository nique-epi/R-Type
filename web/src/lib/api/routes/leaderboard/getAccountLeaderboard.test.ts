import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { getAccountLeaderboard } from './getAccountLeaderboard';

const ROW = {
  rank: 1,
  nickname: 'ACE',
  score: 2234,
  lastPlayedAt: 1800000000,
};

describe('getAccountLeaderboard', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the accounts of the leaderboard, with no filter', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { accounts: [] }));
    vi.stubGlobal('fetch', fetch);

    await getAccountLeaderboard({ limit: 10 });

    const url = new URL(String(fetch.mock.calls[0]?.[0]));
    expect(url.pathname).toBe('/api/leaderboard/accounts');
    expect(Object.fromEntries(url.searchParams)).toEqual({ limit: '10' });
  });

  it('returns the lines of the answer, in the order of the master', async () => {
    const accounts = [ROW, { ...ROW, rank: 2, nickname: 'BOB', score: 2000 }];
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(answer(200, { accounts })),
    );

    expect(await getAccountLeaderboard({ limit: 10 })).toEqual({
      data: accounts,
      error: null,
    });
  });

  it('rejects a line whose total is not a number', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(
          answer(200, { accounts: [{ ...ROW, score: 'a lot' }] }),
        ),
    );

    expect((await getAccountLeaderboard({ limit: 10 })).error).toBe(
      'ERR_API_VALIDATION_FAILED',
    );
  });
});
