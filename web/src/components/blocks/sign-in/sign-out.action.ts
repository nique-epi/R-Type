'use server';

import { api } from '@/lib/api';
import { actionClient } from '@/lib/safe-action';
import { forgetSession, readSessionToken } from '@/lib/session/session-cookie';

/**
 * Signs the visitor out: closes its session on the master, then forgets the
 * cookie, whether the master answered or not, so the browser holds no token
 * once the visitor asked to leave.
 */
export const signOutAction = actionClient.action(async () => {
  const token = await readSessionToken();
  if (token) {
    await api.sessions.deleteOwnSession(token);
  }
  await forgetSession();
  return { status: 'signedOut' as const };
});
