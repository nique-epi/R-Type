import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { bearerHeadersOf } from '../../helpers/bearer-headers';

/**
 * Closes the session of `token` on the master with `DELETE /api/sessions/me`;
 * closing it again succeeds.
 */
export async function deleteOwnSession(
  token: string,
): Promise<ApiResponse<true>> {
  const { error } = await master('@delete/sessions/me', {
    headers: bearerHeadersOf(token),
  });

  if (error) {
    return handleApiError<true>(error);
  }

  return { data: true, error: null };
}
