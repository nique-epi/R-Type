import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { Sample } from '../../schemas';

/** The newest samples of the server `serverId`, newest first, at most `limit`. */
export async function getSamples(
  serverId: number,
  limit: number,
): Promise<ApiResponse<Sample[]>> {
  const { data, error } = await master('@get/servers/:id/samples', {
    params: { id: String(serverId) },
    query: { limit },
  });

  if (error) {
    return handleApiError<Sample[]>(error);
  }

  return { data: data.samples, error: null };
}
