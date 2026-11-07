'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

export type ParallaxProps = React.ComponentProps<'div'> & {
  /** How far it drifts, in percent of its own height, across the scroll: -6 to 6 by default. */
  range?: number;
};

/**
 * Drifts against the scroll while it crosses the screen, so it seems to lie
 * deeper than the page: the scene of a video card, the art of a feature.
 */
export function Parallax({ range = 6, className, ...props }: ParallaxProps) {
  const element = useRef<HTMLDivElement>(null);

  useMotion(() => {
    gsap.fromTo(
      element.current,
      { yPercent: -range },
      {
        yPercent: range,
        ease: 'none',
        scrollTrigger: {
          trigger: element.current,
          start: 'top bottom',
          end: 'bottom top',
          scrub: true,
        },
      },
    );
  });

  return (
    <div ref={element} data-slot="parallax" className={className} {...props} />
  );
}
