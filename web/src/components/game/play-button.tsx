'use client';

import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { useRef } from 'react';
import { Triangle } from '@/components/primitives/triangle';
import { Ship } from '@/components/sprites/ship';
import { motionAllowed, useMotion } from '@/hooks/use-motion';
import { gsap } from '@/lib/gsap';
import { cn } from '@/lib/utils';

const CHEVRONS = 3;

const playButtonVariants = cva(
  'cut-8 group/play relative box-border inline-flex cursor-pointer items-center overflow-hidden whitespace-nowrap border-0 bg-hud-cyan font-pixel text-ink uppercase disabled:cursor-default disabled:bg-deep-blue disabled:text-blue',
  {
    variants: {
      size: {
        md: 'h-12 gap-3 pr-[18px] pl-3 text-base leading-4',
        lg: 'h-16 gap-4 pr-[26px] pl-4 text-2xl leading-6',
      },
    },
    defaultVariants: { size: 'md' },
  },
);

export type PlayButtonProps = Omit<React.ComponentProps<'button'>, 'onClick'> &
  VariantProps<typeof playButtonVariants> & {
    /** Draws the ship of pilot 1 at the start of the button, which fires on hover. */
    ship?: boolean;
    /** An icon at the start of the button, when it has no ship. */
    icon?: React.ReactNode;
    /** Called on a click, once the ship has flown off when it has one. */
    onLaunch?: () => void;
  };

/**
 * The button that starts something, in the look of the game: cut corners,
 * a cyan fill and three chevrons that blink while it waits. On hover the
 * ship nudges forward and fires; on a click it flashes and flies off before
 * `onLaunch` runs.
 */
export function PlayButton({
  ship = false,
  icon,
  size,
  onLaunch,
  className,
  children,
  type = 'button',
  ...props
}: PlayButtonProps) {
  const button = useRef<HTMLButtonElement>(null);
  const large = size === 'lg';

  const { contextSafe } = useMotion(
    () => {
      gsap.to('[data-chevron]', {
        opacity: 0.2,
        duration: 0.25,
        ease: 'steps(1)',
        stagger: { each: 0.15, repeat: -1, yoyo: true },
      });
    },
    { scope: button },
  );

  const hover = contextSafe(() => {
    if (button.current?.disabled) {
      return;
    }
    gsap.to('[data-play-ship]', { x: 6, duration: 0.15, ease: 'steps(3)' });
    gsap.fromTo(
      '[data-play-beam]',
      { width: 0, opacity: 1 },
      {
        width: 48,
        opacity: 0,
        duration: 0.3,
        ease: 'steps(6)',
        repeat: 1,
        repeatDelay: 0.1,
      },
    );
    gsap.to('[data-chevron]', {
      x: 4,
      duration: 0.1,
      ease: 'steps(2)',
      stagger: 0.05,
      yoyo: true,
      repeat: 3,
    });
  });

  const leave = contextSafe(() => {
    gsap.to('[data-play-ship]', { x: 0, duration: 0.15, ease: 'steps(3)' });
  });

  const launch = contextSafe(() => {
    if (!ship || !motionAllowed()) {
      onLaunch?.();
      return;
    }
    gsap
      .timeline({ onComplete: () => onLaunch?.() })
      .to(button.current, {
        filter: 'brightness(1.8)',
        duration: 0.06,
        ease: 'steps(1)',
        yoyo: true,
        repeat: 3,
      })
      .to('[data-play-ship]', { x: 360, duration: 0.3, ease: 'steps(8)' }, 0.1)
      .set('[data-play-ship]', { x: 0 });
  });

  return (
    <button
      ref={button}
      type={type}
      data-slot="play-button"
      onMouseEnter={hover}
      onMouseLeave={leave}
      onClick={launch}
      className={cn(playButtonVariants({ size }), className)}
      {...props}
    >
      <span
        aria-hidden="true"
        className="cut-6 absolute inset-0.5 bg-edge-blue group-disabled/play:bg-sky"
      />
      {ship ? (
        <>
          <Ship
            data-play-ship
            player={1}
            scale={large ? 2 : 1}
            className="relative"
          />
          <span
            data-play-beam
            aria-hidden="true"
            className={cn(
              'absolute top-1/2 h-1 w-0 -translate-y-1/2 bg-star-white',
              large ? 'left-21 h-1.5' : 'left-11',
            )}
          />
        </>
      ) : null}
      {icon ? <span className="relative">{icon}</span> : null}
      <span className="relative">{children}</span>
      <span aria-hidden="true" className="relative inline-flex gap-[3px]">
        {Array.from({ length: CHEVRONS }, (_, index) => (
          <Triangle
            // biome-ignore lint/suspicious/noArrayIndexKey: the chevrons are identical and never reorder.
            key={index}
            data-chevron
            size={large ? 'md' : 'sm'}
            className={large ? undefined : 'border-l-7'}
          />
        ))}
      </span>
    </button>
  );
}
