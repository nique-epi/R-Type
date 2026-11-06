'use client';

import { useRouter } from 'next/navigation';
import { useEffect } from 'react';

/** Renders the page again on the server every interval while the tab is visible. */
export function AutoRefresh({ intervalMs }: { intervalMs: number }) {
  const router = useRouter();

  useEffect(() => {
    function refreshWhenVisible() {
      if (document.visibilityState === 'visible') {
        router.refresh();
      }
    }

    const timer = setInterval(refreshWhenVisible, intervalMs);
    document.addEventListener('visibilitychange', refreshWhenVisible);

    return () => {
      clearInterval(timer);
      document.removeEventListener('visibilitychange', refreshWhenVisible);
    };
  }, [router, intervalMs]);

  return null;
}
