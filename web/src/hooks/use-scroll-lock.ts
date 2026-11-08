'use client';

import { useLenis } from 'lenis/react';
import { useEffect } from 'react';

/**
 * Stops the page from scrolling while `locked`: the smooth scroll of Lenis
 * when it runs, the native scroll of the page otherwise (Lenis does not run
 * when the reader asked for reduced motion).
 */
export function useScrollLock(locked: boolean) {
  const lenis = useLenis();

  useEffect(() => {
    if (!locked) {
      return;
    }
    const root = document.documentElement;
    const overflow = root.style.overflow;
    lenis?.stop();
    root.style.overflow = 'hidden';
    return () => {
      lenis?.start();
      root.style.overflow = overflow;
    };
  }, [locked, lenis]);
}
