'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

/** How many steps the line takes to write itself, as a line of the HUD. */
const TYPE_STEPS = 14;

/**
 * Its child writes itself in from left to right, in whole steps, as a line
 * of the HUD. The child is wrapped in a block that is revealed, so any
 * heading can be passed.
 */
export function TypeIn({
  children,
  delay = 0.1,
  duration = 0.9,
}: {
  children: React.ReactNode;
  delay?: number;
  duration?: number;
}) {
  const element = useRef<HTMLDivElement>(null);

  useMotion(() => {
    gsap.fromTo(
      element.current,
      { clipPath: 'inset(0 100% 0 0)' },
      {
        clipPath: 'inset(0 0% 0 0)',
        duration,
        delay,
        ease: `steps(${TYPE_STEPS})`,
      },
    );
  });

  return (
    <div ref={element} data-slot="type-in">
      {children}
    </div>
  );
}
