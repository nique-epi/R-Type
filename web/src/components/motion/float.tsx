'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';

export type FloatProps = React.ComponentProps<'span'> & {
  /** How far it rises, in pixels. */
  distance?: number;
  /** Seconds of one rise. */
  duration?: number;
  delay?: number;
  /** 'sine.inOut' for a smooth hover, 'steps(6)' for the stepped bob of a sprite. */
  ease?: string;
};

/** Holds a sprite that hovers up and down for as long as it is on the page. */
export function Float({
  distance = 18,
  duration = 1.8,
  delay = 0,
  ease = 'sine.inOut',
  className,
  ...props
}: FloatProps) {
  const element = useRef<HTMLSpanElement>(null);

  useMotion(() => {
    gsap.to(element.current, {
      y: -distance,
      duration,
      delay,
      ease,
      repeat: -1,
      yoyo: true,
    });
  });

  return (
    <span
      ref={element}
      data-slot="float"
      className={cn('inline-block', className)}
      {...props}
    />
  );
}
