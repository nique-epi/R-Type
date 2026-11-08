'use client';

import { useRouter } from 'next/navigation';
import { useEffect } from 'react';

const MILLISECONDS_PER_SECOND = 1000;

/** How often the pages that show the servers read the master again, in seconds. */
export const SERVERS_REFRESH_SECONDS = 15;
export const SERVER_SHEET_REFRESH_SECONDS = 10;
export const LEADERBOARD_REFRESH_SECONDS = 30;

/**
 * Makes the page read its data again every `seconds`, so what it shows
 * follows the master without the reader reloading. The server renders the
 * page again and the browser swaps what changed: a choice the reader made in
 * a client component (a star picked on the map, a filter) stays as it is. A
 * tab that is not on screen waits, and reads again as soon as it is shown.
 */
export function AutoRefresh({ seconds }: { seconds: number }) {
  const router = useRouter();

  useEffect(() => {
    function refreshWhenShown() {
      if (document.visibilityState === 'visible') {
        router.refresh();
      }
    }

    const timer = setInterval(
      refreshWhenShown,
      seconds * MILLISECONDS_PER_SECOND,
    );
    document.addEventListener('visibilitychange', refreshWhenShown);

    return () => {
      clearInterval(timer);
      document.removeEventListener('visibilitychange', refreshWhenShown);
    };
  }, [router, seconds]);

  return null;
}
