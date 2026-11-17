import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { visitorHeadersOf } from '../../helpers/visitor-headers';
import type { User } from '../../schemas';

export type UserCreationRequest = {
  /** The nickname, already checked by `credentialsSchema`. */
  nickname: string;
  /** The password, already checked by `credentialsSchema`. */
  password: string;
  /** The address of the visitor who asks, or `null` when the site knows none. */
  visitorAddress: string | null;
};

/**
 * Creates an account on the master with `POST /api/users`. A nickname taken is
 * `CONFLICT`, a quota reached `TOO_MANY_REQUESTS`, and a master reached over
 * plain HTTP `FORBIDDEN`. The password goes to the master alone: it is kept
 * nowhere and written in no journal.
 */
export async function createUser({
  nickname,
  password,
  visitorAddress,
}: UserCreationRequest): Promise<ApiResponse<User>> {
  const { data, error } = await master('@post/users', {
    body: { nickname, password },
    headers: visitorHeadersOf(visitorAddress),
  });

  if (error) {
    return handleApiError<User>(error);
  }

  return { data, error: null };
}
