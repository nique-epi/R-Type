import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { decideLinkAction } from './link.action';

const request = vi.hoisted(() => ({ token: undefined as string | undefined }));

vi.mock('next/headers', () => ({
  cookies: async () => ({
    get: () => (request.token ? { value: request.token } : undefined),
  }),
}));

const STATE = {
  userCode: 'KQFB-WZPM',
  status: 'approved',
  createdAt: 1_800_000_000,
  expiresAt: 1_800_000_600,
  interval: 5,
};

function masterAnswers(response: Response) {
  const fetch = vi.fn().mockResolvedValue(response);
  vi.stubGlobal('fetch', fetch);
  return fetch;
}

describe('decideLinkAction', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.token = undefined;
  });

  it('asks nothing of the master for a visitor with no session', async () => {
    const fetch = masterAnswers(answer(200, STATE));

    const result = await decideLinkAction({
      userCode: 'KQFB-WZPM',
      decision: 'approved',
    });

    expect(result?.data).toEqual({ status: 'signedOut' });
    expect(fetch).not.toHaveBeenCalled();
  });

  it('connects the game for a signed-in visitor, the code as the master keeps it', async () => {
    request.token = 'rts_0123';
    const fetch = masterAnswers(answer(200, STATE));

    const result = await decideLinkAction({
      userCode: 'kqfbwzpm',
      decision: 'approved',
    });

    expect(result?.data).toEqual({ status: 'approved' });
    const [input] = fetch.mock.calls[0] as [unknown];
    expect(String(input)).toContain('/api/device-links/KQFB-WZPM/decision');
  });

  it('says a link decided otherwise was decided', async () => {
    request.token = 'rts_0123';
    masterAnswers(answer(409, { status: 409, title: 'Conflict' }));

    const result = await decideLinkAction({
      userCode: 'KQFB-WZPM',
      decision: 'denied',
    });

    expect(result?.data).toEqual({ status: 'decided' });
  });
});
