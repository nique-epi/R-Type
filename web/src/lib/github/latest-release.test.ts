import { afterEach, describe, expect, it, vi } from 'vitest';

import { fetchLatestRelease } from './latest-release';
import { RELEASE_V0_1_0 } from './release.fixture';

function answer(status: number, body: unknown) {
  return new Response(JSON.stringify(body), { status });
}

describe('fetchLatestRelease', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the latest release of the repository of GITHUB_REPOSITORY', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, RELEASE_V0_1_0));
    vi.stubGlobal('fetch', fetch);

    await fetchLatestRelease();

    expect(String(fetch.mock.calls[0]?.[0])).toBe(
      'https://api.github.com/repos/nique-epi/R-Type/releases/latest',
    );
  });

  it('returns the release of the answer', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(answer(200, RELEASE_V0_1_0)),
    );

    expect((await fetchLatestRelease())?.version).toBe('0.1.0');
  });

  it('is nothing when the limit of the API is reached', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(answer(403, { message: 'rate limit' })),
    );

    expect(await fetchLatestRelease()).toBeNull();
  });

  it('is nothing when GitHub cannot be reached', async () => {
    vi.stubGlobal('fetch', vi.fn().mockRejectedValue(new TypeError('failed')));

    expect(await fetchLatestRelease()).toBeNull();
  });
});
