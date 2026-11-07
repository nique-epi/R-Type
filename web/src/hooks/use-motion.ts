'use client';

import type { RefObject } from 'react';
import { gsap, MOTION_ALLOWED, useGSAP } from '@/lib/gsap';

export type MotionConfig = {
  /** Selector text in the animation only reaches the descendants of this element. */
  scope?: RefObject<Element | null>;
  /** The values that run the animation again when they change. */
  dependencies?: unknown[];
  /** Undo the previous run before the next one, when a dependency changes. */
  revertOnUpdate?: boolean;
  /**
   * A media query the animation also needs, such as a wide screen. The
   * animation then starts and reverts as the query starts and stops matching.
   */
  query?: string;
};

/**
 * Runs GSAP animations through `useGSAP()`, which reverts them when the
 * component goes, and only when the reader allows motion: under
 * `prefers-reduced-motion: reduce` nothing runs and the markup stays as
 * rendered, in its final state. The preference is read each time the
 * animation runs, in the context of `useGSAP()` itself, so a run on a new
 * dependency adds no context of its own. Returns `contextSafe`, for the
 * animations an event handler starts later.
 */
export function useMotion(
  animate: gsap.ContextFunc,
  {
    scope,
    dependencies = [],
    revertOnUpdate = false,
    query,
  }: MotionConfig = {},
) {
  return useGSAP(
    (context, contextSafe) => {
      if (query === undefined) {
        return motionAllowed() ? animate(context, contextSafe) : undefined;
      }
      const media = gsap.matchMedia();
      media.add(`${query} and ${MOTION_ALLOWED}`, animate);
      return () => media.revert();
    },
    { scope, dependencies, revertOnUpdate },
  );
}

/** Whether the reader allows motion now, for an animation an event handler starts. */
export function motionAllowed(): boolean {
  return window.matchMedia(MOTION_ALLOWED).matches;
}
