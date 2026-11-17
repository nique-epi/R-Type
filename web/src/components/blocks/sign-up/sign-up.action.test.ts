import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { signUpAction } from './sign-up.action';

const request = vi.hoisted(() => ({ headers: new Headers() }));

vi.mock('next/headers', () => ({ headers: async () => request.headers }));

const PASSWORD = 'correct horse battery staple';
const CREATED = { id: 1, nickname: 'ACE', role: 'player' };

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

function signUp(nickname: unknown = 'ACE', password: unknown = PASSWORD) {
  return signUpAction({ nickname, password } as never);
}

describe('signUpAction', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.headers = new Headers();
  });

  it('returns the account the master created', async () => {
    masterAnswers(answer(201, CREATED));

    expect((await signUp())?.data).toEqual({
      status: 'created',
      user: { id: 1, nickname: 'ACE' },
    });
  });

  it('sends the nickname without the blanks around it', async () => {
    const fetch = masterAnswers(answer(201, CREATED));

    await signUp('  ACE ');

    const [, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(JSON.parse(String(init.body))).toEqual({
      nickname: 'ACE',
      password: PASSWORD,
    });
  });

  it('names the visitor the host of the site saw', async () => {
    request.headers = new Headers({ 'x-forwarded-for': '203.0.113.7' });
    const fetch = masterAnswers(answer(201, CREATED));

    await signUp();

    const [, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(new Headers(init.headers).get('X-Site-Client-Address')).toBe(
      '203.0.113.7',
    );
  });

  it.each([
    ['an empty nickname', '', PASSWORD],
    ['a nickname of 17 bytes', 'a'.repeat(17), PASSWORD],
    ['a password of 7 bytes', 'ACE', 'a'.repeat(7)],
    ['a password of 129 bytes', 'ACE', 'a'.repeat(129)],
    ['what is no text', { nickname: 'ACE' }, PASSWORD],
  ])(
    'refuses %s without asking the master',
    async (_case, nickname, password) => {
      const fetch = masterAnswers(answer(201, CREATED));

      const result = await signUp(nickname, password);

      expect(result?.validationErrors).toBeDefined();
      expect(result?.data).toBeUndefined();
      expect(fetch).not.toHaveBeenCalled();
    },
  );

  it.each([
    [409, 'Conflict', 'taken'],
    [429, 'Too Many Requests', 'quota'],
    [400, 'Bad Request', 'refused'],
    [403, 'Forbidden', 'unreachable'],
    [500, 'Internal Server Error', 'unreachable'],
  ])('reads the %i of the master', async (status, title, outcome) => {
    masterAnswers(answer(status, { status, title }));

    expect((await signUp())?.data).toEqual({ status: outcome });
  });

  it('says the master cannot be reached when nothing answers', async () => {
    vi.stubGlobal(
      'fetch',
      vi.fn().mockRejectedValue(new TypeError('fetch failed')),
    );

    expect((await signUp())?.data).toEqual({ status: 'unreachable' });
  });

  it('answers an error it did not expect without writing it anywhere', async () => {
    const written = vi.spyOn(console, 'error').mockImplementation(() => {});
    request.headers = undefined as unknown as Headers;
    masterAnswers(answer(201, CREATED));

    const result = await signUp();

    expect(result?.serverError).toBe('UNEXPECTED_ERROR');
    expect(written).not.toHaveBeenCalled();
    written.mockRestore();
  });

  it('writes nothing in the journal, the password least of all', async () => {
    const written = ['log', 'info', 'warn', 'error', 'debug'].map(method =>
      vi.spyOn(console, method as 'log').mockImplementation(() => {}),
    );
    masterAnswers(answer(201, CREATED));

    await signUp();
    await signUp('ACE', 'a'.repeat(7));

    for (const spy of written) {
      expect(spy).not.toHaveBeenCalled();
      spy.mockRestore();
    }
  });
});
