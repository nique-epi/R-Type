import { afterEach, describe, expect, it, vi } from 'vitest';

import { answer } from './answer';
import { getStatuses } from './getStatuses';

describe('getStatuses', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('returns the statuses of the answer, newest first', async () => {
    const statuses = [
      { status: 'online', since: 1800000100 },
      { status: 'offline', since: 1800000000 },
    ];
    const fetch = vi.fn().mockResolvedValue(answer(200, { statuses }));
    vi.stubGlobal('fetch', fetch);

    expect(await getStatuses(7)).toEqual({ data: statuses, error: null });
    expect(String(fetch.mock.calls[0]?.[0])).toBe(
      'http://master.test:8080/api/servers/7/statuses',
    );
  });
});
