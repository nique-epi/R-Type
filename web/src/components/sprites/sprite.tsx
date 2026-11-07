import type * as React from 'react';
import { cn } from '@/lib/utils';
import type { SpriteFrame, SpriteSheet } from './sheets';

/** Frames laid side by side on the sheet, played in a loop from the first one. */
export type SpriteAnimation = {
  frames: number;
  /** Seconds of one loop. */
  seconds: number;
  /** Seconds before it starts; a negative delay starts it partway through. */
  delay?: number;
};

export type SpriteProps = Omit<React.ComponentProps<'span'>, 'children'> & {
  sheet: SpriteSheet;
  frame: SpriteFrame;
  /** Screen pixels per pixel of the sheet: a whole number, so no pixel blurs. */
  scale: number;
  /** The scale under 861px wide, when the sprite must shrink on a phone. */
  narrowScale?: number;
  /** Names the sprite for a screen reader; without it, it is decoration. */
  label?: string;
  /** Plays the frames to the right of `frame`, unless the reader asked for reduced motion. */
  animation?: SpriteAnimation;
};

/**
 * One frame of a sheet of the game, drawn at a whole scale. Every sprite of
 * the site goes through it: the frame and the scale are data, so a new size
 * needs no new CSS.
 */
export function Sprite({
  sheet,
  frame,
  scale,
  narrowScale,
  label,
  animation,
  className,
  style,
  ...props
}: SpriteProps) {
  const geometry = {
    '--sprite-sheet': `url(${sheet.src})`,
    '--sheet-w': sheet.width,
    '--sheet-h': sheet.height,
    '--sprite-x': frame.x,
    '--sprite-y': frame.y,
    '--sprite-w': frame.width,
    '--sprite-h': frame.height,
    '--sprite-scale': scale,
    '--sprite-scale-narrow': narrowScale,
    '--sprite-frames': animation?.frames,
    '--sprite-animation': animation
      ? `sprite-frames ${animation.seconds}s steps(${animation.frames}) ${animation.delay ?? 0}s infinite`
      : undefined,
  } as React.CSSProperties;

  return (
    <span
      data-slot="sprite"
      className={cn('sprite', className)}
      style={{ ...geometry, ...style }}
      {...(label
        ? { role: 'img', 'aria-label': label }
        : { 'aria-hidden': true })}
      {...props}
    />
  );
}
