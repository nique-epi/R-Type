import 'server-only';

import { createSafeActionClient } from 'next-safe-action';

/** What an action answers for an error it did not expect, whatever the error. */
export const UNEXPECTED_ACTION_ERROR = 'UNEXPECTED_ERROR';

/**
 * The client every server action of the site is built from: an action checks
 * its input against its schema before it runs, and answers an error it throws
 * with UNEXPECTED_ACTION_ERROR alone. The error is never written in the
 * journal nor sent to the browser, since it may hold what the visitor typed.
 */
export const actionClient = createSafeActionClient({
  handleServerError() {
    return UNEXPECTED_ACTION_ERROR;
  },
});
