'use client';

import { type LenisRef, ReactLenis, useLenis } from 'lenis/react';
import type * as React from 'react';
import { useEffect, useRef, useState } from 'react';
import { gsap, MOTION_ALLOWED, ScrollTrigger } from '@/lib/gsap';

const MILLISECONDS_PER_SECOND = 1000;

/** How far the scroll moves toward its target each frame; the mockup's value. */
const SCROLL_LERP = 0.09;

/** Tells ScrollTrigger each time Lenis moves the page. */
function ScrollTriggerSync() {
  useLenis(() => ScrollTrigger.update());
  return null;
}

/**
 * Smooth scroll for the whole page (Lenis), driven by the ticker of GSAP so
 * ScrollTrigger reads the same scroll position the page draws. Mounted once,
 * in the root layout. A reader who asked for reduced motion keeps the native
 * scroll of their browser.
 */
export function SmoothScroll({ children }: { children: React.ReactNode }) {
  const lenis = useRef<LenisRef>(null);
  const [smooth, setSmooth] = useState(false);

  useEffect(() => {
    const query = window.matchMedia(MOTION_ALLOWED);
    const follow = () => setSmooth(query.matches);
    follow();
    query.addEventListener('change', follow);
    return () => query.removeEventListener('change', follow);
  }, []);

  useEffect(() => {
    if (!smooth) {
      return;
    }
    function step(time: number) {
      lenis.current?.lenis?.raf(time * MILLISECONDS_PER_SECOND);
    }
    gsap.ticker.add(step);
    gsap.ticker.lagSmoothing(0);
    return () => gsap.ticker.remove(step);
  }, [smooth]);

  if (!smooth) {
    return children;
  }
  return (
    <ReactLenis
      root
      ref={lenis}
      options={{ autoRaf: false, lerp: SCROLL_LERP, smoothWheel: true }}
    >
      <ScrollTriggerSync />
      {children}
    </ReactLenis>
  );
}
