import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { bearerHeadersOf } from '../../helpers/bearer-headers';
import type { DeviceLinkDecision, DeviceLinkState } from '../../schemas';
import type { DeviceLinkRequest } from './readDeviceLink';

export type DeviceLinkDecisionRequest = DeviceLinkRequest & {
  decision: DeviceLinkDecision;
};

/**
 * Writes the decision of the signed-in player on a device link with
 * `PUT /api/device-links/{code}/decision`. The same decision sent again is
 * answered the same; another one, or a link expired undecided, is `CONFLICT`.
 */
export async function decideDeviceLink({
  userCode,
  token,
  decision,
}: DeviceLinkDecisionRequest): Promise<ApiResponse<DeviceLinkState>> {
  const { data, error } = await master('@put/device-links/:code/decision', {
    params: { code: userCode },
    body: { decision },
    headers: bearerHeadersOf(token),
  });

  if (error) {
    return handleApiError<DeviceLinkState>(error);
  }

  return { data, error: null };
}
