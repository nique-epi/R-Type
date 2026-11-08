'use client';

import { type RefObject, useCallback, useEffect, useState } from 'react';

/**
 * Whether `target` is the element the browser shows fullscreen, and the
 * switch that enters or leaves it. A browser without the API, or one that
 * refuses the request, leaves the page as it is.
 */
export function useFullscreen(target: RefObject<HTMLElement | null>) {
  const [active, setActive] = useState(false);

  useEffect(() => {
    function sync() {
      setActive(document.fullscreenElement === target.current);
    }
    document.addEventListener('fullscreenchange', sync);
    return () => document.removeEventListener('fullscreenchange', sync);
  }, [target]);

  const toggle = useCallback(() => {
    if (document.fullscreenElement) {
      void document.exitFullscreen();
      return;
    }
    target.current?.requestFullscreen?.().catch(() => undefined);
  }, [target]);

  return { active, toggle };
}
