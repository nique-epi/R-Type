import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer, listedServer } from '@/test/responses';
import { listServers } from './listServers';

const FULL_PAGE = 100;

function servers(count: number, firstId: number) {
  return Array.from({ length: count }, (_, index) =>
    listedServer(firstId + index),
  );
}

describe('listServers', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('reads the next page while a page comes back full', async () => {
    const fetch = vi
      .fn()
      .mockResolvedValueOnce(answer(200, { servers: servers(FULL_PAGE, 1) }))
      .mockResolvedValueOnce(answer(200, { servers: servers(2, 101) }));
    vi.stubGlobal('fetch', fetch);

    const response = await listServers();

    expect(response.data?.length).toBe(FULL_PAGE + 2);
    expect(fetch.mock.calls.map(call => String(call[0]))).toEqual([
      'http://master.test:8080/api/servers?limit=100&offset=0',
      'http://master.test:8080/api/servers?limit=100&offset=100',
    ]);
  });

  it('stops after one page that is not full', async () => {
    const fetch = vi
      .fn()
      .mockResolvedValue(answer(200, { servers: servers(3, 1) }));
    vi.stubGlobal('fetch', fetch);

    await listServers();

    expect(fetch).toHaveBeenCalledTimes(1);
  });

  it('reads at most ten pages', async () => {
    const fetch = vi
      .fn()
      .mockImplementation(async () =>
        answer(200, { servers: servers(FULL_PAGE, 1) }),
      );
    vi.stubGlobal('fetch', fetch);

    await listServers();

    expect(fetch).toHaveBeenCalledTimes(10);
  });

  it('reports the error of a page and keeps none of the servers', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValueOnce(answer(200, { servers: servers(FULL_PAGE, 1) }))
        .mockResolvedValueOnce(
          answer(500, { status: 500, title: 'Internal Server Error' }),
        ),
    );

    expect(await listServers()).toEqual({
      data: null,
      error: 'INTERNAL_SERVER_ERROR',
    });
  });

  it('rejects a server whose status the master does not know', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(
        answer(200, {
          servers: [{ ...listedServer(1), status: 'sleeping' }],
        }),
      ),
    );

    expect((await listServers()).error).toBe('ERR_API_VALIDATION_FAILED');
  });
});
