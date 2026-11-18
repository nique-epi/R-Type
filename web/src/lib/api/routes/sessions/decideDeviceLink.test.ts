import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from '@/test/responses';
import { decideDeviceLink } from './decideDeviceLink';

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

describe('decideDeviceLink', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('puts the decision on the link of the code, the session as Bearer', async () => {
    const fetch = masterAnswers(answer(200, STATE));

    const response = await decideDeviceLink({
      userCode: 'KQFB-WZPM',
      token: 'rts_0123',
      decision: 'approved',
    });

    expect(response).toEqual({ data: STATE, error: null });
    const [input, init] = fetch.mock.calls[0] as [unknown, RequestInit];
    expect(String(input)).toBe(
      'http://master.test:8080/api/device-links/KQFB-WZPM/decision',
    );
    expect(init.method).toBe('PUT');
    expect(new Headers(init.headers).get('Authorization')).toBe(
      'Bearer rts_0123',
    );
    expect(JSON.parse(String(init.body))).toEqual({ decision: 'approved' });
  });

  it('maps a link decided otherwise to CONFLICT', async () => {
    masterAnswers(answer(409, { status: 409, title: 'Conflict' }));

    const response = await decideDeviceLink({
      userCode: 'KQFB-WZPM',
      token: 'rts_0123',
      decision: 'denied',
    });

    expect(response.error).toBe('CONFLICT');
  });
});
