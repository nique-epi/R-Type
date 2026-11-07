import type * as React from 'react';
import { cn } from '@/lib/utils';

export type CutFrameProps = React.ComponentProps<'div'> & {
  /** The classes of the inside, under the 2-pixel edge: its fill and its layout. */
  innerClassName?: string;
  /** How deep the outer corners are cut: 8 pixels for a panel, 6 for a small box. */
  corners?: 8 | 6;
};

/**
 * A frame of the game client: corners cut 8 pixels deep, a 2-pixel edge in
 * the color of the outer box (`bg-…` in `className`) around an inside cut
 * 6 pixels deep (`bg-…` in `innerClassName`). The clip path draws the edge,
 * so it stays crisp at any size.
 */
export function CutFrame({
  className,
  innerClassName,
  corners = 8,
  children,
  ...props
}: CutFrameProps) {
  return (
    <div
      data-slot="cut-frame"
      className={cn(
        'relative bg-edge-blue',
        corners === 8 ? 'cut-8' : 'cut-6',
        className,
      )}
      {...props}
    >
      <div className={cn('cut-6 absolute inset-0.5 bg-sky', innerClassName)}>
        {children}
      </div>
    </div>
  );
}
