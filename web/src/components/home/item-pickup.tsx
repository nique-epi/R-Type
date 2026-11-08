'use client';

import type * as React from 'react';
import { useRef } from 'react';
import { DressedShip } from '@/components/sprites/dressed-ship';
import { lookItemSprite, SHEETS } from '@/components/sprites/sheets';
import { Sprite } from '@/components/sprites/sprite';
import { useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import type { LookItem } from '@/lib/rtype/ship-looks';

/** Screen pixels per pixel of the game, the scale of a ship in a game. */
const PICKUP_SCALE = 4;

export type ItemPickupProps = {
  item: LookItem;
  /** "CROWN WON", over the ship once it touched the item. */
  won: React.ReactNode;
  /** Says what the scene shows, for a screen reader. */
  label: string;
};

/**
 * A ship flying into an item that floats in from the right, then wearing it.
 * The markup is the end of the scene, the ship wearing the item; the timeline
 * plays it from the item floating in, again and again.
 */
export function ItemPickup({ item, won, label }: ItemPickupProps) {
  const stage = useRef<HTMLDivElement>(null);
  const { frame } = lookItemSprite(item);

  useMotion(
    () => {
      const floating = '[data-pickup="floating"]';
      const worn = '[data-pickup="worn"]';
      const banner = '[data-pickup="won"]';
      gsap
        .timeline({ repeat: -1, repeatDelay: 0.6 })
        .set(worn, { autoAlpha: 0 })
        .set(banner, { autoAlpha: 0, y: 12 })
        .fromTo(
          floating,
          { left: '100%', autoAlpha: 1 },
          { left: '0%', duration: 2.6, ease: 'none' },
        )
        .set(floating, { autoAlpha: 0 })
        .set(worn, { autoAlpha: 1 })
        .to(banner, { autoAlpha: 1, y: 0, duration: 0.3, ease: 'steps(4)' })
        .to(banner, { autoAlpha: 0, duration: 0.3, delay: 1.6 })
        .to({}, { duration: 0.6 });
    },
    { scope: stage },
  );

  return (
    <div
      ref={stage}
      role="img"
      aria-label={label}
      data-slot="item-pickup"
      className="starfield relative flex h-[220px] items-center overflow-hidden rounded-[32px] pl-10 max-md:h-[180px] max-md:pl-5"
    >
      <span className="relative inline-flex flex-none items-center">
        <DressedShip
          player={1}
          item={item}
          scale={PICKUP_SCALE}
          itemProps={{ 'data-pickup': 'worn' } as React.ComponentProps<'span'>}
        />
        <span
          data-pickup="won"
          className="absolute -top-8 left-full ml-6 whitespace-nowrap font-pixel text-sm text-star-gold uppercase"
        >
          {won}
        </span>
      </span>
      {/* The way the item floats, from the right edge to the nose of the ship. */}
      <span className="relative ml-2 h-full flex-1">
        <Sprite
          sheet={SHEETS.looks}
          frame={frame}
          scale={PICKUP_SCALE}
          data-pickup="floating"
          className="invisible absolute left-0"
          style={{ top: `calc(50% + ${2 * PICKUP_SCALE}px)` }}
        />
      </span>
    </div>
  );
}
