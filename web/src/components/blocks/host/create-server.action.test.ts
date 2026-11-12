import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { createServerAction } from './create-server.action';

const request = vi.hoisted(() => ({ headers: new Headers() }));

vi.mock('next/headers', () => ({ headers: async () => request.headers }));

const KEY = `rtk_${'a1'.repeat(32)}`;
const CREATED = { id: 7, name: 'Nebula Lyon', key: KEY };

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('createServerAction', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.headers = new Headers();
  });

  it('returns the server and its key to the page', async () => {
    masterAnswers(answer(201, CREATED));

    expect(await createServerAction('Nebula Lyon')).toEqual({
      status: 'created',
      server: CREATED,
    });
  });

  it('sends the name without the blanks around it', async () => {
    const fetch = masterAnswers(answer(201, CREATED));

    await createServerAction('  Nebula Lyon ');

    const [, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(JSON.parse(String(init.body))).toEqual({ name: 'Nebula Lyon' });
  });

  it('names the visitor the host of the site saw', async () => {
    request.headers = new Headers({ 'x-forwarded-for': '203.0.113.7' });
    const fetch = masterAnswers(answer(201, CREATED));

    await createServerAction('Nebula Lyon');

    const [, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(new Headers(init.headers).get('X-Site-Client-Address')).toBe(
      '203.0.113.7',
    );
  });

  it.each([
    ['an empty name', ''],
    ['a name of blanks', '   '],
    ['a name of 33 bytes', 'a'.repeat(33)],
    ['what is no text', { name: 'Nebula Lyon' }],
  ])('refuses %s without asking the master', async (_case, name) => {
    const fetch = masterAnswers(answer(201, CREATED));

    expect(await createServerAction(name)).toEqual({ status: 'invalidName' });
    expect(fetch).not.toHaveBeenCalled();
  });

  it('says the quota is reached on a 429', async () => {
    masterAnswers(answer(429, { status: 429, title: 'Too Many Requests' }));

    expect(await createServerAction('Nebula Lyon')).toEqual({
      status: 'quota',
    });
  });

  it('says the master refused the name on a 400', async () => {
    masterAnswers(answer(400, { status: 400, title: 'Bad Request' }));

    expect(await createServerAction('Nebula Lyon')).toEqual({
      status: 'refused',
    });
  });

  it('says the master cannot be reached when nothing answers', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockRejectedValue(new TypeError('fetch failed')),
    );

    expect(await createServerAction('Nebula Lyon')).toEqual({
      status: 'unreachable',
    });
  });

  it('says the master cannot be reached on an answer that is no server', async () => {
    masterAnswers(answer(500, { status: 500, title: 'Internal Server Error' }));

    expect(await createServerAction('Nebula Lyon')).toEqual({
      status: 'unreachable',
    });
  });

  it('writes nothing in the journal, the key least of all', async () => {
    const written = ['log', 'info', 'warn', 'error', 'debug'].map(method =>
      vi.spyOn(console, method as 'log').mockImplementation(() => {}),
    );
    masterAnswers(answer(201, CREATED));

    await createServerAction('Nebula Lyon');

    for (const spy of written) {
      expect(spy).not.toHaveBeenCalled();
      spy.mockRestore();
    }
  });
});
