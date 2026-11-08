'use client';

import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';

/** The three layers of stars of the game sky, back to front. */
export const STAR_LAYERS = ['far', 'mid', 'near'] as const;

export type StarLayer = (typeof STAR_LAYERS)[number];

/**
 * Each layer is a 480 × 270 picture of the client drawn at ×4 and repeated;
 * the seconds it takes to cross its width are the speeds of STAR_LAYERS in
 * the client, so the near stars pass faster than the far ones.
 */
const LAYER_LOOKS: Record<
  StarLayer,
  { image: string; secondsPerWidth: number }
> = {
  far: { image: '/sky/stars-far.png', secondsPerWidth: 107 },
  mid: { image: '/sky/stars-mid.png', secondsPerWidth: 40 },
  near: { image: '/sky/stars-near.png', secondsPerWidth: 16 },
};

const LAYER_WIDTH = 1920;

export type StarLayersProps = {
  layers?: readonly StarLayer[];
  className?: string;
};

/**
 * The night sky of a game, its layers of stars drifting left at their own
 * depth. It fills its positioned parent and takes no pointer events.
 */
export function StarLayers({
  layers = STAR_LAYERS,
  className,
}: StarLayersProps) {
  const root = useRef<HTMLDivElement>(null);

  useMotion(
    () => {
      for (const layer of layers) {
        gsap.fromTo(
          `[data-layer=${layer}]`,
          { x: 0 },
          {
            x: -LAYER_WIDTH,
            duration: LAYER_LOOKS[layer].secondsPerWidth,
            ease: 'none',
            repeat: -1,
          },
        );
      }
    },
    { scope: root },
  );

  return (
    <div
      ref={root}
      aria-hidden="true"
      data-slot="star-layers"
      className={cn(
        'pointer-events-none absolute inset-0 overflow-hidden',
        className,
      )}
    >
      {layers.map(layer => (
        <div
          key={layer}
          data-layer={layer}
          className="pixelated absolute inset-y-0 left-0 w-[calc(100%+1920px)] bg-repeat bg-size-[1920px_1080px] will-change-transform"
          style={{ backgroundImage: `url(${LAYER_LOOKS[layer].image})` }}
        />
      ))}
    </div>
  );
}
