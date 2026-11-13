'use server';

import { headers } from 'next/headers';

import { api } from '@/lib/api';
import type { ServerCreationOutcome } from '@/lib/rtype/server-creation';
import { serverNameProblemOf } from '@/lib/rtype/server-name';
import { visitorAddressOf } from '@/lib/visitor-address';

/** The header the host of the site names the visitor of a request in. */
const FORWARDED_FOR_HEADER = 'x-forwarded-for';

/**
 * The server action of the hosting page: creates a server named `name` on
 * the master and returns it with its key to the page that asked, alone. The
 * key is written in no journal, in no address and in no store of the site:
 * once the page is gone, nobody can read it again.
 *
 * A browser can call an action with anything, so the name is checked again
 * here: one the form would refuse never reaches the master.
 */
export async function createServerAction(
  name: unknown,
): Promise<ServerCreationOutcome> {
  if (typeof name !== 'string' || serverNameProblemOf(name) !== null) {
    return { status: 'invalidName' };
  }

  const { data, error } = await api.servers.createServer({
    name: name.trim(),
    visitorAddress: visitorAddressOf(
      (await headers()).get(FORWARDED_FOR_HEADER),
    ),
  });

  if (data) {
    return { status: 'created', server: data };
  }
  if (error === 'TOO_MANY_REQUESTS') {
    return { status: 'quota' };
  }

  return { status: error === 'BAD_REQUEST' ? 'refused' : 'unreachable' };
}
