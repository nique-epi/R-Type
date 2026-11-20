import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { bearerHeadersOf } from '../../helpers/bearer-headers';
import type { DeviceLinkState } from '../../schemas';

export type DeviceLinkRequest = {
  /** The short code of the link, as the master keeps it. */
  userCode: string;
  /** The token of the session of the player who reads it. */
  token: string;
};

/**
 * Reads the device link of a short code with
 * `GET /api/device-links/{code}`, as the signed-in player sees it before
 * deciding. No link of that code is `NOT_FOUND`, a session gone
 * `UNAUTHORIZED`.
 */
export async function readDeviceLink({
  userCode,
  token,
}: DeviceLinkRequest): Promise<ApiResponse<DeviceLinkState>> {
  const { data, error } = await master('@get/device-links/:code', {
    params: { code: userCode },
    headers: bearerHeadersOf(token),
  });

  if (error) {
    return handleApiError<DeviceLinkState>(error);
  }

  return { data, error: null };
}
