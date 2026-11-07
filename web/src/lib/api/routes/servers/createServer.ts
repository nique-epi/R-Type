import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { CreatedServer } from '../../schemas';

/**
 * Creates a game server named `name`. The answer holds its key, which the
 * master shows this once: the caller must not cache or log it.
 */
export async function createServer(
  name: string,
): Promise<ApiResponse<CreatedServer>> {
  const { data, error } = await master('@post/servers', { body: { name } });

  if (error) {
    return handleApiError<CreatedServer>(error);
  }

  return { data, error: null };
}
