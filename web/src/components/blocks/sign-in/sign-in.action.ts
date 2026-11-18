'use server';

import { api } from '@/lib/api';
import { requestVisitorAddress } from '@/lib/request-visitor-address';
import { type SignInOutcome, signInFormSchema } from '@/lib/rtype/sign-in';
import { actionClient } from '@/lib/safe-action';
import { keepSession } from '@/lib/session/session-cookie';

/**
 * The server action of the sign-in page: opens a session on the master for
 * the visitor who asks, and keeps its token in the cookie of the site. The
 * password is sent to the master alone, and kept nowhere; the token never
 * reaches the page.
 */
export const signInAction = actionClient
  .inputSchema(signInFormSchema)
  .action(async ({ parsedInput }): Promise<SignInOutcome> => {
    const { data, error } = await api.sessions.createSession({
      ...parsedInput,
      visitorAddress: await requestVisitorAddress(),
    });

    if (data) {
      await keepSession(data.token, data.expiresAt);
      return { status: 'signedIn', nickname: data.account.nickname };
    }
    if (error === 'UNAUTHORIZED') {
      return { status: 'wrong' };
    }

    return { status: error === 'TOO_MANY_REQUESTS' ? 'quota' : 'unreachable' };
  });
