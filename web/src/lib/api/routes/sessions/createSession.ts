import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { visitorHeadersOf } from '../../helpers/visitor-headers';
import type { CreatedSession } from '../../schemas';

export type SessionRequest = {
  /** The nickname, already checked by `signInFormSchema`. */
  nickname: string;
  /** The password, sent to the master alone. */
  password: string;
  /** The address of the visitor who signs in, or `null` when the site knows none. */
  visitorAddress: string | null;
};

/**
 * Opens a session on the master with `POST /api/sessions`. A wrong nickname
 * or password is `UNAUTHORIZED`, too many failures from the visitor
 * `TOO_MANY_REQUESTS`. The password is kept nowhere and written in no journal;
 * the token comes back once.
 */
export async function createSession({
  nickname,
  password,
  visitorAddress,
}: SessionRequest): Promise<ApiResponse<CreatedSession>> {
  const { data, error } = await master('@post/sessions', {
    body: { nickname, password },
    headers: visitorHeadersOf(visitorAddress),
  });

  if (error) {
    return handleApiError<CreatedSession>(error);
  }

  return { data, error: null };
}
