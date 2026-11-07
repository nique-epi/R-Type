import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { Sample } from '../../schemas';

/** The most samples one page holds (`MAXIMUM_PAGE_SIZE` of the master). */
const MAXIMUM_SAMPLES = 100;

/** The newest samples of the server `serverId`, newest first, at most `limit`. */
export async function getSamples(
  serverId: number,
  limit: number = MAXIMUM_SAMPLES,
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
