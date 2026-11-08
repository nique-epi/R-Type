'use client';

import type * as React from 'react';
import { useEffect, useRef, useState } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';

/** The picture of the client is 480 × 270; the site shows it at ×2. */
export const GAME_SCREEN_WIDTH = 960;
export const GAME_SCREEN_HEIGHT = 540;

/** The smallest scale the screen shrinks to before its box scrolls sideways. */
const MINIMUM_SCALE = 0.35;

export type GameScreenProps = Omit<React.ComponentProps<'div'>, 'role'> & {
  /** What the screen is and the keys it takes, read when it gets the focus. */
  'aria-label': string;
  /** Also fit the height of the window, as the full-page sector map does. */
  fitHeight?: boolean;
  /** Grow past the size of the picture to fill the box, for a fullscreen screen. */
  upscale?: boolean;
  /** Settle into place, tilted then flat, as it scrolls to the middle of the window. */
  settle?: boolean;
};

/**
 * The scale that fits the screen in a box of that width (and that height, if
 * given). It never grows past the 960 × 540 of the picture unless `upscale`
 * says the box may be larger, as a screen shown fullscreen on a wall display.
 */
export function fittingScale(
  width: number,
  height?: number,
  upscale = false,
): number {
  const across = width / GAME_SCREEN_WIDTH;
  const down = height === undefined ? across : height / GAME_SCREEN_HEIGHT;
  const fitting = Math.min(across, down);
  return Math.max(MINIMUM_SCALE, upscale ? fitting : Math.min(1, fitting));
}

/**
 * A screen of the game client drawn in the page: its children are laid on a
 * 960 × 540 canvas in pixels of the client at ×2, and the whole screen
 * scales down to the width it is given. It takes the focus, so its keys work
 * as they do in the game.
 */
export function GameScreen({
  fitHeight = false,
  upscale = false,
  settle = false,
  className,
  children,
  ...props
}: GameScreenProps) {
  const box = useRef<HTMLDivElement>(null);
  const [scale, setScale] = useState(1);

  useEffect(() => {
    const element = box.current;
    if (element === null) {
      return;
    }
    function measure() {
      if (element === null) {
        return;
      }
      const top = Math.max(0, element.getBoundingClientRect().top);
      setScale(
        fittingScale(
          element.clientWidth,
          fitHeight ? window.innerHeight - top : undefined,
          upscale,
        ),
      );
    }
    measure();
    const observer = new ResizeObserver(measure);
    observer.observe(element);
    return () => observer.disconnect();
  }, [fitHeight, upscale]);

  useMotion(
    () => {
      if (!settle) {
        return;
      }
      gsap.fromTo(
        box.current,
        { scale: 0.92, rotationX: 8, transformPerspective: 1200 },
        {
          scale: 1,
          rotationX: 0,
          ease: 'none',
          scrollTrigger: {
            trigger: box.current,
            start: 'top bottom',
            end: 'top 35%',
            scrub: 0.6,
          },
        },
      );
    },
    { dependencies: [settle] },
  );

  return (
    <div
      ref={box}
      data-slot="game-screen"
      className="w-full overflow-x-auto pb-1"
    >
      <div
        className="relative mx-auto"
        style={{
          width: GAME_SCREEN_WIDTH * scale,
          height: GAME_SCREEN_HEIGHT * scale,
        }}
      >
        <div
          role="application"
          // biome-ignore lint/a11y/noNoninteractiveTabindex: the screen takes the keys of the game, as role="application" tells a screen reader, so it must take the focus.
          tabIndex={0}
          className={cn(
            'absolute top-0 left-0 origin-top-left overflow-hidden rounded-xl outline-none focus-visible:shadow-[0_0_0_2px_var(--color-star-white)]',
            className,
          )}
          style={{
            width: GAME_SCREEN_WIDTH,
            height: GAME_SCREEN_HEIGHT,
            transform: `scale(${scale})`,
          }}
          {...props}
        >
          {children}
        </div>
      </div>
    </div>
  );
}
