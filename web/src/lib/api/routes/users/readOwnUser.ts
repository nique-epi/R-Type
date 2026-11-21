import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { bearerHeadersOf } from '../../helpers/bearer-headers';
import type { User } from '../../schemas';

/**
 * Reads the account of the session of `token` with `GET /api/users/me`. A
 * session gone, or never opened, is `UNAUTHORIZED`.
 */
export async function readOwnUser(token: string): Promise<ApiResponse<User>> {
  const { data, error } = await master('@get/users/me', {
    headers: bearerHeadersOf(token),
  });

  if (error) {
    return handleApiError<User>(error);
  }

  return { data, error: null };
}
