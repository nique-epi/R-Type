import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { visitorHeadersOf } from '../../helpers/visitor-headers';
import type { CreatedServer } from '../../schemas';

export type ServerCreationRequest = {
  /** The name players will read, already checked by `serverNameProblemOf()`. */
  name: string;
  /** The address of the visitor who asks, or `null` when the site knows none. */
  visitorAddress: string | null;
};

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
