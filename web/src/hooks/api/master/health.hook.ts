'use client';

import { useEffect, useState } from 'react';
import { api } from '@/lib/api';
import type { ApiResponse } from '@/lib/api/helpers';
import type { Health } from '@/lib/api/schemas';

export const HEALTH_REFRESH_INTERVAL_MS = 5_000;

/** What the page knows of the master: not asked yet, answering, or silent. */
export type MasterHealth =
  | { state: 'checking' }
  | { state: 'online'; version: string }
  | { state: 'unreachable' };

/** Turns an answer of the health route into what the page shows. */
export function masterHealthOf(response: ApiResponse<Health>): MasterHealth {
  if (response.error) {
    return { state: 'unreachable' };
  }

  return { state: 'online', version: response.data.version };
}

/** Asks the master for its health on mount, then every refresh interval. */
export function useMasterHealth(): MasterHealth {
  const [health, setHealth] = useState<MasterHealth>({ state: 'checking' });

  useEffect(() => {
    let mounted = true;

    async function refresh() {
      const response = await api.health.getHealth();

      if (mounted) {
        setHealth(masterHealthOf(response));
      }
    }

    void refresh();
    const timer = setInterval(refresh, HEALTH_REFRESH_INTERVAL_MS);

    return () => {
      mounted = false;
      clearInterval(timer);
    };
  }, []);

  return health;
}
