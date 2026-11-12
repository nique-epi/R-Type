import { env } from '@/config/server-env';

import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { CreatedServer } from '../../schemas';

/** The header the site names itself to the master in, with `MASTER_SITE_KEY`. */
export const SITE_KEY_HEADER = 'X-Site-Key';

/** The header the site names the visitor it creates a server for in. */
export const SITE_CLIENT_ADDRESS_HEADER = 'X-Site-Client-Address';

export type ServerCreationRequest = {
  /** The name players will read, already checked by `serverNameProblemOf()`. */
  name: string;
  /** The address of the visitor who asks, or `null` when the site knows none. */
  visitorAddress: string | null;
};

/**
 * Who the site creates the server for: its key and the address of the
 * visitor, so the master counts the quota of that visitor and not the one of
 * the site. Nothing without both: the master then counts the address it sees.
 */
function visitorHeadersOf(
  visitorAddress: string | null,
): Record<string, string> {
  if (!env.MASTER_SITE_KEY || !visitorAddress) {
    return {};
  }

  return {
    [SITE_KEY_HEADER]: env.MASTER_SITE_KEY,
    [SITE_CLIENT_ADDRESS_HEADER]: visitorAddress,
  };
}

/**
 * Creates a server on the master with `POST /api/servers` and returns it with
 * its key, which the master shows this once: the caller hands it to the page
 * and keeps it nowhere. A quota reached is `TOO_MANY_REQUESTS`.
 */
export async function createServer({
  name,
  visitorAddress,
}: ServerCreationRequest): Promise<ApiResponse<CreatedServer>> {
  const { data, error } = await master('@post/servers', {
    body: { name },
    headers: visitorHeadersOf(visitorAddress),
  });

  if (error) {
    return handleApiError<CreatedServer>(error);
  }

  return { data, error: null };
}
