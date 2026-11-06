import 'server-only';

import { cache } from 'react';
import { type ApiResponse, api, type Health } from '@/lib/api';

/** What the page knows of the master: answering with its version, or silent. */
export type MasterHealth =
  | { state: 'online'; version: string }
  | { state: 'unreachable' };

/** Turns an answer of the health route into what the page shows. */
export function masterHealthOf(response: ApiResponse<Health>): MasterHealth {
  if (response.error) {
    return { state: 'unreachable' };
  }

  return { state: 'online', version: response.data.version };
}

/** Asks the master for its health once per request of the page. */
export const getMasterHealth = cache(
  async (): Promise<MasterHealth> =>
    masterHealthOf(await api.health.getHealth()),
);
