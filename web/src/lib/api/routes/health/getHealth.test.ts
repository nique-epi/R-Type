import { afterEach, describe, expect, it, vi } from 'vitest';

import { getHealth } from './getHealth';

function answer(status: number, body: unknown) {
  return new Response(JSON.stringify(body), {
    status,
    headers: { 'Content-Type': 'application/json' },
  });
}

describe('getHealth', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the health route of the master API on the same origin', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(200, { version: '0.1.0' }));
    vi.stubGlobal('fetch', fetch);

    await getHealth();

    expect(String(fetch.mock.calls[0]?.[0])).toBe('/api/health');
  });

  it('returns the version the master answers', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(answer(200, { version: '0.1.0' })),
    );

    expect(await getHealth()).toEqual({
      data: { version: '0.1.0' },
      error: null,
    });
  });

  it('rejects an answer without a version', async () => {
    vi.stubGlobal('fetch', vi.fn().mockResolvedValue(answer(200, {})));

    expect((await getHealth()).error).toBe('ERR_API_VALIDATION_FAILED');
  });

  it('maps a problem document of the master to its error code', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(answer(404, { status: 404, title: 'Not Found' })),
    );

    expect((await getHealth()).error).toBe('NOT_FOUND');
  });

  it('reports a master that cannot be reached', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockRejectedValue(new TypeError('fetch failed')),
    );

    expect((await getHealth()).error).toBe('ERR_API_FETCH_FAILED');
  });
});
