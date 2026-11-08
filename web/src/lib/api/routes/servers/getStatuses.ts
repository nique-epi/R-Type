import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { StatusChange } from '../../schemas';

/** The statuses the server `serverId` held, newest first. */
export async function getStatuses(
  serverId: number,
): Promise<ApiResponse<StatusChange[]>> {
  const { data, error } = await master('@get/servers/:id/statuses', {
    params: { id: String(serverId) },
  });

  if (error) {
    return handleApiError<StatusChange[]>(error);
  }

  return { data: data.statuses, error: null };
}
