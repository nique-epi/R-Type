import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { createServer } from './createServer';

const KEY = `rtk_${'a1'.repeat(32)}`;
const CREATED = { id: 7, name: 'Paris 1', key: KEY };

/** The request `fetch` was called with first: its URL and what it sent. */
function sentBy(fetch: ReturnType<typeof vi.fn>) {
  const [input, init] = fetch.mock.calls[0] as [unknown, RequestInit];
  return { url: String(input), init, headers: new Headers(init.headers) };
}

describe('createServer', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    vi.unstubAllEnvs();
    vi.resetModules();
  });

  it('posts the name and returns the server with its key', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(201, CREATED));
    vi.stubGlobal('fetch', fetch);

    const response = await createServer({
      name: 'Paris 1',
      visitorAddress: null,
    });

    expect(response).toEqual({ data: CREATED, error: null });
    const { url, init } = sentBy(fetch);
    expect(url).toBe('http://master.test:8080/api/servers');
    expect(init.method).toBe('POST');
    expect(JSON.parse(String(init.body))).toEqual({ name: 'Paris 1' });
  });

  it('names the site and the visitor it creates the server for', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(201, CREATED));
    vi.stubGlobal('fetch', fetch);

    await createServer({ name: 'Paris 1', visitorAddress: '203.0.113.7' });

    const { headers } = sentBy(fetch);
    expect(headers.get('X-Site-Key')).toBe('site-key-of-the-tests');
    expect(headers.get('X-Site-Client-Address')).toBe('203.0.113.7');
  });

  it('does not name the site without a visitor to name', async () => {
    const fetch = vi.fn().mockResolvedValue(answer(201, CREATED));
    vi.stubGlobal('fetch', fetch);

    await createServer({ name: 'Paris 1', visitorAddress: null });

    const { headers } = sentBy(fetch);
    expect(headers.has('X-Site-Key')).toBe(false);
    expect(headers.has('X-Site-Client-Address')).toBe(false);
  });

  it('names no visitor while the site has no key', async () => {
    vi.stubEnv('MASTER_SITE_KEY', undefined);
    vi.resetModules();
    const fetch = vi.fn().mockResolvedValue(answer(201, CREATED));
    vi.stubGlobal('fetch', fetch);
    const { createServer: createWithoutKey } = await import('./createServer');

    await createWithoutKey({ name: 'Paris 1', visitorAddress: '203.0.113.7' });

    const { headers } = sentBy(fetch);
    expect(headers.has('X-Site-Key')).toBe(false);
    expect(headers.has('X-Site-Client-Address')).toBe(false);
  });

  it('reports the quota of the master', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(
          answer(429, { status: 429, title: 'Too Many Requests' }),
        ),
    );

    expect(
      await createServer({ name: 'Paris 1', visitorAddress: null }),
    ).toEqual({ data: null, error: 'TOO_MANY_REQUESTS' });
  });

  it('reports a master that cannot be reached', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockRejectedValue(new TypeError('fetch failed')),
    );

    expect(
      (await createServer({ name: 'Paris 1', visitorAddress: null })).error,
    ).toBe('ERR_API_FETCH_FAILED');
  });

  it('rejects an answer whose key is not a key of the master', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(answer(201, { ...CREATED, key: 'secret' })),
    );

    expect(
      (await createServer({ name: 'Paris 1', visitorAddress: null })).error,
    ).toBe('ERR_API_VALIDATION_FAILED');
  });
});
