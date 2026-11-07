'use client';

import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

export type CountUpProps = {
  value: number;
  duration?: number;
  delay?: number;
  /** 'power2.out' for a count of the site, 'steps(20)' for a score of a game screen. */
  ease?: string;
  className?: string;
};

/**
 * A number that climbs from 0 to its value when it first shows. The page
 * renders the value itself, so it reads right before the script runs, to a
 * screen reader, and without motion.
 */
export function CountUp({
  value,
  duration = 1,
  delay = 0,
  ease = 'power2.out',
  className,
}: CountUpProps) {
  const element = useRef<HTMLSpanElement>(null);

  useMotion(
    () => {
      const counter = { value: 0 };
      gsap.to(counter, {
        value,
        duration,
        delay,
        ease,
        onUpdate: () => {
          const shown = String(Math.round(counter.value));
          if (element.current && element.current.textContent !== shown) {
            element.current.textContent = shown;
          }
        },
      });
      return () => {
        if (element.current) {
          element.current.textContent = String(value);
        }
      };
    },
    { dependencies: [value], revertOnUpdate: true },
  );

  return (
    <span ref={element} className={className}>
      {value}
    </span>
  );
}
