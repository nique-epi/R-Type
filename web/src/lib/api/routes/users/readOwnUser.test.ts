import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { readOwnUser } from './readOwnUser';

const TOKEN = 'rts_0123';
const ACCOUNT = { id: 1, nickname: 'ACE', role: 'player' };

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('readOwnUser', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('reads the account of the session, naming it as Bearer', async () => {
    const fetch = masterAnswers(answer(200, ACCOUNT));

    const response = await readOwnUser(TOKEN);

    expect(response).toEqual({ data: ACCOUNT, error: null });
    const [input, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(String(input)).toBe('http://master.test:8080/api/users/me');
    expect(new Headers(init.headers).get('Authorization')).toBe(
      `Bearer ${TOKEN}`,
    );
  });

  it('maps a session the master no longer knows to UNAUTHORIZED', async () => {
    masterAnswers(answer(401, { status: 401, title: 'Unauthorized' }));

    const response = await readOwnUser(TOKEN);

    expect(response.error).toBe('UNAUTHORIZED');
  });
});
