import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from './answer';
import { getSamples } from './getSamples';

describe('getSamples', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the newest samples, as many as the limit it is given', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { samples: [] }));
    vi.stubGlobal('fetch', fetch);

    await getSamples(7, 1);

    expect(String(fetch.mock.calls[0]?.[0])).toBe(
      'http://master.test:8080/api/servers/7/samples?limit=1',
    );
  });

  it('keeps the ping a sample carries and accepts one without measures', async () => {
    const samples = [
      {
        takenAt: 1800000010,
        gameCount: 2,
        playerCount: 3,
        measures: { averagePingMilliseconds: 42 },
      },
      { takenAt: 1800000000, gameCount: 0, playerCount: 0, measures: {} },
    ];
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue(answer(200, { samples })));

    expect(await getSamples(7)).toEqual({ data: samples, error: null });
  });
});
