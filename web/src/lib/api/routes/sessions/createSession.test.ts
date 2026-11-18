import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { createSession } from './createSession';

const PASSWORD = 'correct horse battery staple';
const SESSION = {
  token: 'rts_0123',
  expiresAt: 1_800_604_800,
  account: { id: 1, nickname: 'ACE', role: 'player' },
};

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('createSession', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('posts the credentials and returns the session', async () => {
    const fetch = masterAnswers(answer(201, SESSION));

    const response = await createSession({
      nickname: 'ACE',
      password: PASSWORD,
      visitorAddress: null,
    });

    expect(response).toEqual({ data: SESSION, error: null });
    const [input, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(String(input)).toBe('http://master.test:8080/api/sessions');
    expect(init.method).toBe('POST');
  });

  it('maps wrong credentials to UNAUTHORIZED', async () => {
    masterAnswers(answer(401, { status: 401, title: 'Unauthorized' }));

    const response = await createSession({
      nickname: 'ACE',
      password: PASSWORD,
      visitorAddress: null,
    });

    expect(response.error).toBe('UNAUTHORIZED');
  });
});
