import type * as React from 'react';
import { cn } from '@/lib/utils';
import type { SpriteFrame, SpriteSheet } from './sheets';

export type SpriteProps = Omit<React.ComponentProps<'span'>, 'children'> & {
  sheet: SpriteSheet;
  frame: SpriteFrame;
  /** Screen pixels per pixel of the sheet: a whole number, so no pixel blurs. */
  scale: number;
  /** The scale under 861px wide, when the sprite must shrink on a phone. */
  narrowScale?: number;
  /** Names the sprite for a screen reader; without it, it is decoration. */
  label?: string;
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
