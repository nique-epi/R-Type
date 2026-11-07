import { afterEach, describe, expect, it, vi } from 'vitest';

import { createServerAction } from './actions';

const KEY = `rtk_${'a'.repeat(64)}`;

function answer(status: number, body: unknown) {
  return new Response(JSON.stringify(body), {
    status,
    headers: { 'Content-Type': 'application/json' },
  });
}

describe('createServerAction', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('asks the master nothing for an empty name', async () => {
    const fetch = vi.fn();
    vi.stubGlobal('fetch', fetch);

    expect(await createServerAction('   ')).toEqual({ status: 'refused' });
    expect(fetch).not.toHaveBeenCalled();
  });

  it('asks the master nothing for a name longer than it takes', async () => {
    const fetch = vi.fn();
    vi.stubGlobal('fetch', fetch);

    expect(await createServerAction('a'.repeat(33))).toEqual({
      status: 'refused',
    });
    expect(fetch).not.toHaveBeenCalled();
  });

  it('sends the name of the longest length, trimmed', async () => {
    const fetch = vi
      .fn()
      .mockResolvedValue(answer(201, { id: 7, name: 'x', key: KEY }));
    vi.stubGlobal('fetch', fetch);

    await createServerAction(` ${'a'.repeat(32)} `);

    expect(JSON.parse(fetch.mock.calls[0]?.[1].body)).toEqual({
      name: 'a'.repeat(32),
    });
  });

  it('gives the server and its key once the master created it', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(answer(201, { id: 7, name: 'Paris 1', key: KEY })),
    );

    expect(await createServerAction('Paris 1')).toEqual({
      status: 'created',
      id: 7,
      name: 'Paris 1',
      key: KEY,
    });
  });

  it('says the quota is reached when the master answers 429', async () => {
    vi.stubGlobal(
      'fetch',
      vi
        .fn()
        .mockResolvedValue(
          answer(429, { status: 429, title: 'Too Many Requests' }),
        ),
    );

    expect(await createServerAction('Paris 1')).toEqual({ status: 'quota' });
  });
});
