import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { MasterGame } from '../../schemas';

/** The games of the last heartbeat of the server `serverId`; none before its first one. */
export async function getGames(
  serverId: number,
): Promise<ApiResponse<MasterGame[]>> {
  const { data, error } = await master('@get/servers/:id/games', {
    params: { id: String(serverId) },
  });

  if (error) {
    return handleApiError<MasterGame[]>(error);
  }

  return { data: data.games, error: null };
}
