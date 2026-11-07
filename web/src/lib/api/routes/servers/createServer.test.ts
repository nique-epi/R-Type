import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { createServer } from './createServer';

const KEY = `rtk_${'a'.repeat(64)}`;

describe('createServer', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('posts the name to /api/servers as JSON', async () => {
    const fetch = vi
      .fn()
      .mockResolvedValue(answer(201, { id: 7, name: 'Paris 1', key: KEY }));
    vi.stubGlobal('fetch', fetch);

    await createServer('Paris 1');

    const [url, init] = fetch.mock.calls[0] ?? [];
    expect(String(url)).toBe('http://master.test:8080/api/servers');
    expect(init.method).toBe('POST');
    expect(JSON.parse(init.body)).toEqual({ name: 'Paris 1' });
  });

  it('returns the server with the key the master shows this once', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(answer(201, { id: 7, name: 'Paris 1', key: KEY })),
    );

    expect(await createServer('Paris 1')).toEqual({
      data: { id: 7, name: 'Paris 1', key: KEY },
      error: null,
    });
  });

  it('maps the quota of five servers an hour to TOO_MANY_REQUESTS', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(
          answer(429, { status: 429, title: 'Too Many Requests' }),
        ),
    );

    expect((await createServer('Paris 1')).error).toBe('TOO_MANY_REQUESTS');
  });
});
