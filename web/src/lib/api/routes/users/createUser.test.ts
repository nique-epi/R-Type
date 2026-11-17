import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { createUser } from './createUser';

const CREATED = { id: 1, nickname: 'ACE', role: 'player' };
const PASSWORD = 'correct horse battery staple';

/** The request `fetch` was called with first: its URL and what it sent. */
function sentBy(fetch: ReturnType<typeof vi.fn>) {
  const [input, init] = fetch.mock.calls[0] as [unknown, RequestInit];
  return { url: String(input), init, headers: new Headers(init.headers) };
}

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('createUser', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('posts the credentials and returns the account', async () => {
    const fetch = masterAnswers(answer(201, CREATED));

    const response = await createUser({
      nickname: 'ACE',
      password: PASSWORD,
      visitorAddress: null,
    });

    expect(response).toEqual({ data: CREATED, error: null });
    const { url, init } = sentBy(fetch);
    expect(url).toBe('http://master.test:8080/api/users');
    expect(init.method).toBe('POST');
    expect(JSON.parse(String(init.body))).toEqual({
      nickname: 'ACE',
      password: PASSWORD,
    });
  });

  it('names the site and the visitor it creates the account for', async () => {
    const fetch = masterAnswers(answer(201, CREATED));

    await createUser({
      nickname: 'ACE',
      password: PASSWORD,
      visitorAddress: '203.0.113.7',
    });

    const { headers } = sentBy(fetch);
    expect(headers.get('X-Site-Key')).toBe('site-key-of-the-tests');
    expect(headers.get('X-Site-Client-Address')).toBe('203.0.113.7');
  });

  it.each([
    [409, 'Conflict', 'CONFLICT'],
    [429, 'Too Many Requests', 'TOO_MANY_REQUESTS'],
    [403, 'Forbidden', 'FORBIDDEN'],
  ])('reports the %i of the master', async (status, title, code) => {
    masterAnswers(answer(status, { status, title }));

    expect(
      await createUser({
        nickname: 'ACE',
        password: PASSWORD,
        visitorAddress: null,
      }),
    ).toEqual({ data: null, error: code });
  });
});
