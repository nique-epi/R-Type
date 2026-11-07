'use client';

import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

const HEX_DIGITS = '0123456789abcdef';

export type ScrambleTextProps = {
  text: string;
  /** The characters drawn while a place is not settled yet. */
  characters?: string;
  /** Characters that never scramble, such as the underscores of a key. */
  keep?: string;
  duration?: number;
  delay?: number;
};

/** Mixes the characters not settled yet; the first `settled` stay as they are. */
export function scrambled(
  text: string,
  settled: number,
  characters: string,
  keep: string,
  random: () => number = Math.random,
): string {
  let result = text.slice(0, settled);
  for (const character of text.slice(settled)) {
    result += keep.includes(character)
      ? character
      : (characters[Math.floor(random() * characters.length)] ?? character);
  }
  return result;
}

/**
 * Text that appears scrambled, then settles one character at a time from
 * the left, as the server key does when it is created.
 */
export function ScrambleText({
  text,
  characters = HEX_DIGITS,
  keep = '_',
  duration = 1.2,
  delay = 0.2,
}: ScrambleTextProps) {
  const element = useRef<HTMLSpanElement>(null);

  useMotion(
    () => {
      const progress = { settled: 0 };
      gsap.to(progress, {
        settled: text.length,
        duration,
        delay,
        ease: 'none',
        onUpdate: () => {
          if (element.current) {
            element.current.textContent = scrambled(
              text,
              Math.floor(progress.settled),
              characters,
              keep,
            );
          }
        },
      });
      return () => {
        if (element.current) {
          element.current.textContent = text;
        }
      };
    },
    { dependencies: [text], revertOnUpdate: true },
  );

  return <span ref={element}>{text}</span>;
}
