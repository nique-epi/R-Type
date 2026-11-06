'use client';

import { useEffect, useState } from 'react';
import { type ApiResponse, api, type Health } from '@/lib/api';

const HEALTH_REFRESH_INTERVAL_MS = 5_000;

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

/** Asks the master for its health, again after each answer while the tab is visible. */
export function useMasterHealth(): MasterHealth {
  const [health, setHealth] = useState<MasterHealth>({ state: 'checking' });

  useEffect(() => {
    let mounted = true;
    let timer: ReturnType<typeof setTimeout> | undefined;
    let asking = false;

    async function refresh() {
      asking = true;
      const response = await api.health.getHealth();
      asking = false;

      if (!mounted) {
        return;
      }
      setHealth(masterHealthOf(response));
      if (document.visibilityState === 'visible') {
        timer = setTimeout(refresh, HEALTH_REFRESH_INTERVAL_MS);
      }
    }

    function refreshWhenShown() {
      if (document.visibilityState === 'visible' && !asking) {
        clearTimeout(timer);
        void refresh();
      }
    }

    void refresh();
    document.addEventListener('visibilitychange', refreshWhenShown);

    return () => {
      mounted = false;
      clearTimeout(timer);
      document.removeEventListener('visibilitychange', refreshWhenShown);
    };
  }, []);

  return health;
}
