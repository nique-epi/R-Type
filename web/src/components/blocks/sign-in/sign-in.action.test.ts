import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { signInAction } from './sign-in.action';

const request = vi.hoisted(() => ({
  headers: new Headers(),
  cookies: new Map<string, { value: string; options: unknown }>(),
}));

vi.mock('next/headers', () => ({
  headers: async () => request.headers,
  cookies: async () => ({
    get: (name: string) => request.cookies.get(name),
    set: (name: string, value: string, options: unknown) =>
      request.cookies.set(name, { value, options }),
    delete: (name: string) => request.cookies.delete(name),
  }),
}));

const PASSWORD = 'correct horse battery staple';
const SESSION = {
  token: 'rts_0123',
  expiresAt: 1_800_604_800,
  account: { id: 1, nickname: 'ACE', role: 'player' },
};

function masterAnswers(response: Response) {
  vi.stubGlobal('fetch', vi.fn().mockResolvedValue(response));
}

describe('signInAction', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.cookies.clear();
  });

  it('keeps the token in a cookie no script reads, over HTTPS only', async () => {
    masterAnswers(answer(201, SESSION));

    const result = await signInAction({ nickname: 'ACE', password: PASSWORD });

    expect(result?.data).toEqual({ status: 'signedIn', nickname: 'ACE' });
    expect(request.cookies.get('rtype_session')).toEqual({
      value: 'rts_0123',
      options: expect.objectContaining({
        httpOnly: true,
        secure: true,
        sameSite: 'lax',
        expires: new Date(1_800_604_800_000),
      }),
    });
  });

  it('keeps no cookie for credentials the master refuses', async () => {
    masterAnswers(answer(401, { status: 401, title: 'Unauthorized' }));

    const result = await signInAction({ nickname: 'ACE', password: PASSWORD });

    expect(result?.data).toEqual({ status: 'wrong' });
    expect(request.cookies.size).toBe(0);
  });
});
