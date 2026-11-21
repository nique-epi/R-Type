import 'server-only';

import { cache } from 'react';
import { api } from '@/lib/api';
import type { User } from '@/lib/api/schemas';
import { readSessionToken } from './session-cookie';

/**
 * The account of the session of the visitor; `null` for a guest, for a
 * session the master no longer knows, and for a master out of reach. The
 * header then offers to sign in, which is never wrong: signing in again only
 * opens a new session. It is read once per request, however many parts of
 * a page ask.
 */
export const currentAccount = cache(async (): Promise<User | null> => {
  const token = await readSessionToken();
  if (!token) {
    return null;
  }

  const { data } = await api.users.readOwnUser(token);
  return data ?? null;
});
