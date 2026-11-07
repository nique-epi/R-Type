'use server';

import { api } from '@/lib/api';
import {
  type CreationOutcome,
  creationOutcomeOf,
} from '@/lib/rtype/creation-outcome';
import { SERVER_NAME_MAXIMUM } from '@/lib/rtype/server-name';

/**
 * Creates a game server on the master for the name a host typed. A name the
 * form would not have sent (empty, or longer than the master takes) is
 * refused without asking the master, since a form can be bypassed. The key
 * of the answer reaches the host's page and nothing else: it is never logged.
 */
export async function createServerAction(
  name: string,
): Promise<CreationOutcome> {
  const trimmed = name.trim();

  if (trimmed === '' || trimmed.length > SERVER_NAME_MAXIMUM) {
    return { status: 'refused' };
  }

  return creationOutcomeOf(await api.servers.createServer(trimmed));
}
