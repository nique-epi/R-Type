'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';

/** The fills of the tiles, in turn: deep blue, the planet's purple, the dark blue of pilot 1. */
const TILE_FILLS = [
  'bg-deep-blue',
  'bg-planet-2',
  'bg-player-dark-blue',
] as const;

/** Seconds for the strip to slide by one copy of its tiles. */
const LOOP_SECONDS = 50;

export type GalleryStripProps = {
  /** One sprite per tile; the strip repeats them to fill the loop. */
  tiles: readonly React.ReactNode[];
  'aria-label': string;
};

/**
 * A strip of tiles that slides left forever under the hero: the ships and
 * the Bydos of the game. It is decoration; the strip is drawn twice so the
 * loop never shows its seam.
 */
export function GalleryStrip({
  tiles,
  'aria-label': label,
}: GalleryStripProps) {
  const strip = useRef<HTMLElement>(null);

  useMotion(
    () => {
      gsap.to('[data-track]', {
        xPercent: -50,
        duration: LOOP_SECONDS,
        ease: 'none',
        repeat: -1,
      });
    },
    { scope: strip },
  );

  const copies = [...tiles, ...tiles];
  return (
    <section
      ref={strip}
      aria-label={label}
      className="overflow-hidden pt-12 pb-4"
    >
      <div data-track aria-hidden="true" className="flex w-max gap-6">
        {copies.map((tile, index) => (
          <div
            // biome-ignore lint/suspicious/noArrayIndexKey: the tiles are drawn twice in a fixed order and never reorder.
            key={index}
            className={`grid h-[150px] w-60 flex-none place-items-center rounded-3xl ${TILE_FILLS[index % TILE_FILLS.length]}`}
          >
            {tile}
          </div>
        ))}
      </div>
    </section>
  );
}
