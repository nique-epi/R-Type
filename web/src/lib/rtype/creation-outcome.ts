import type { ApiResponse, CreatedServer } from '@/lib/api';

/** What a host is told once the master answered the creation of a server. */
export type CreationOutcome =
  | { status: 'created'; id: number; name: string; key: string }
  /** Five servers an hour per address is the most the master creates. */
  | { status: 'quota' }
  /** The master did not answer. */
  | { status: 'unreachable' }
  /** Any other refusal: the name, the shape of the request. */
  | { status: 'refused' };

/** Turns the answer of the master to a creation into what the page shows. */
export function creationOutcomeOf(
  response: ApiResponse<CreatedServer>,
): CreationOutcome {
  if (response.error === null) {
    return { status: 'created', ...response.data };
  }
  if (response.error === 'TOO_MANY_REQUESTS') {
    return { status: 'quota' };
  }
  return response.error === 'ERR_API_FETCH_FAILED'
    ? { status: 'unreachable' }
    : { status: 'refused' };
}
