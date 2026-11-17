'use server';

import { api } from '@/lib/api';
import { requestVisitorAddress } from '@/lib/request-visitor-address';
import { credentialsSchema, type SignUpOutcome } from '@/lib/rtype/sign-up';
import { actionClient } from '@/lib/safe-action';

/**
 * The server action of the sign-up page: creates the account on the master
 * for the visitor who asks. Its input is checked again against
 * `credentialsSchema`, so credentials the form would refuse never reach the
 * master. The password is sent to the master alone, and kept nowhere.
 */
export const signUpAction = actionClient
  .inputSchema(credentialsSchema)
  .action(async ({ parsedInput }): Promise<SignUpOutcome> => {
    const { data, error } = await api.users.createUser({
      ...parsedInput,
      visitorAddress: await requestVisitorAddress(),
    });

    if (data) {
      return {
        status: 'created',
        user: { id: data.id, nickname: data.nickname },
      };
    }
    if (error === 'CONFLICT') {
      return { status: 'taken' };
    }
    if (error === 'TOO_MANY_REQUESTS') {
      return { status: 'quota' };
    }

    return { status: error === 'BAD_REQUEST' ? 'refused' : 'unreachable' };
  });
