import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { currentAccount } from './current-account';
import { SESSION_COOKIE } from './session-cookie';

const request = vi.hoisted(() => ({
  cookies: new Map<string, { value: string }>(),
}));

vi.mock('next/headers', () => ({
  cookies: async () => ({
    get: (name: string) => request.cookies.get(name),
  }),
}));

const TOKEN = 'rts_0123';
const ACCOUNT = { id: 1, nickname: 'ACE', role: 'player' };

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('currentAccount', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.cookies.clear();
  });

  it('is null for a guest, without asking the master', async () => {
    const fetch = masterAnswers(answer(200, ACCOUNT));

    expect(await currentAccount()).toBeNull();
    expect(fetch).not.toHaveBeenCalled();
  });

  it('is the account of the session the cookie holds', async () => {
    request.cookies.set(SESSION_COOKIE, { value: TOKEN });
    masterAnswers(answer(200, ACCOUNT));

    expect(await currentAccount()).toEqual(ACCOUNT);
  });

  it('is null for a session the master no longer knows', async () => {
    request.cookies.set(SESSION_COOKIE, { value: TOKEN });
    masterAnswers(answer(401, { status: 401, title: 'Unauthorized' }));

    expect(await currentAccount()).toBeNull();
  });
});
