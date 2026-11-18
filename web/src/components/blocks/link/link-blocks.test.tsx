import { afterEach, describe, expect, it, vi } from 'vitest';

import { LinkBlocks } from './link-blocks';

const request = vi.hoisted(() => ({ token: undefined as string | undefined }));

vi.mock('next/headers', () => ({
  cookies: async () => ({
    get: () => (request.token ? { value: request.token } : undefined),
  }),
}));

vi.mock('next/navigation', () => ({
  redirect: (path: string) => {
    throw new Error(`redirect ${path}`);
  },
  useRouter: () => ({ replace: vi.fn(), refresh: vi.fn() }),
}));

describe('LinkBlocks', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
    request.token = undefined;
  });

  it('sends a visitor with no session to the sign-in page, which leads back', async () => {
    await expect(LinkBlocks({ lang: 'fr', code: 'kqfb-wzpm' })).rejects.toThrow(
      'redirect /fr/signin?next=%2Ffr%2Flink%3Fcode%3DKQFB-WZPM',
    );
  });

  it('sends a visitor whose session expired to the sign-in page', async () => {
    request.token = 'rts_0123';
    vi.stubGlobal(
      'fetch',
      vi.fn().mockResolvedValue(
        new Response(JSON.stringify({ status: 401, title: 'Unauthorized' }), {
          status: 401,
          headers: { 'Content-Type': 'application/json' },
        }),
      ),
    );

    await expect(LinkBlocks({ lang: 'en', code: 'KQFB-WZPM' })).rejects.toThrow(
      'redirect /en/signin?next=%2Fen%2Flink%3Fcode%3DKQFB-WZPM',
    );
  });
});
