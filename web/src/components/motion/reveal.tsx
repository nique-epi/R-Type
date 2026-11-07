'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap, ScrollTrigger } from '@/lib/gsap';

export type RevealProps = React.ComponentProps<'div'> & {
  /** Where each child starts, in pixels: it rises (`y`) or slides (`x`) into place. */
  from?: { x?: number; y?: number };
  /** Seconds between two children entering together. */
  stagger?: number;
  duration?: number;
  /** When a child enters, as ScrollTrigger reads it: the mockup's 'top 92%'. */
  start?: string;
  /** Reveal the elements matching this selector rather than the direct children. */
  targets?: string;
};

const DEFAULT_FROM = { y: 40 };

/**
 * Its children rise into view as they scroll in, the ones already on screen
 * at once, each batch staggered. Without motion they are simply there.
 */
export function Reveal({
  from = DEFAULT_FROM,
  stagger = 0.08,
  duration = 0.8,
  start = 'top 92%',
  targets,
  className,
  ...props
}: RevealProps) {
  const root = useRef<HTMLDivElement>(null);

  useMotion(
    () => {
      const element = root.current;
      if (element === null) {
        return;
      }
      const items = targets
        ? gsap.utils.toArray<Element>(targets, element)
        : Array.from(element.children);
      gsap.set(items, { opacity: 0, x: from.x ?? 0, y: from.y ?? 0 });
      ScrollTrigger.batch(items, {
        start,
        once: true,
        onEnter: batch =>
          gsap.to(batch, {
            opacity: 1,
            x: 0,
            y: 0,
            duration,
            stagger,
            ease: 'power3.out',
            overwrite: true,
            clearProps: 'transform',
          }),
      });
    },
    { scope: root },
  );

  return <div ref={root} data-slot="reveal" className={className} {...props} />;
}
