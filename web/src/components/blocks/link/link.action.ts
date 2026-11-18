'use server';

import { z } from 'zod';
import { api } from '@/lib/api';
import { deviceLinkDecisionSchema } from '@/lib/api/schemas';
import { type LinkDecisionOutcome, userCodeOf } from '@/lib/rtype/device-link';
import { actionClient } from '@/lib/safe-action';
import { readSessionToken } from '@/lib/session/session-cookie';

/** What the page sends: the code of the link and the decision of the player. */
const decisionSchema = z.object({
  userCode: z.string().transform((text, context) => {
    const code = userCodeOf(text);
    if (code === null) {
      context.addIssue({ code: 'custom', message: 'userCode' });
      return z.NEVER;
    }
    return code;
  }),
  decision: deviceLinkDecisionSchema,
});

/**
 * The server action of the page `/link`: writes the decision of the
 * signed-in visitor on the device link of a code, with the session kept in
 * the cookie of the site. A visitor with no session is `signedOut`.
 */
export const decideLinkAction = actionClient
  .inputSchema(decisionSchema)
  .action(async ({ parsedInput }): Promise<LinkDecisionOutcome> => {
    const token = await readSessionToken();
    if (!token) {
      return { status: 'signedOut' };
    }
    const { data, error } = await api.sessions.decideDeviceLink({
      ...parsedInput,
      token,
    });

    if (data) {
      return { status: parsedInput.decision };
    }
    switch (error) {
      case 'CONFLICT':
        return { status: 'decided' };
      case 'NOT_FOUND':
        return { status: 'unknown' };
      case 'UNAUTHORIZED':
        return { status: 'signedOut' };
      default:
        return { status: 'unreachable' };
    }
  });
