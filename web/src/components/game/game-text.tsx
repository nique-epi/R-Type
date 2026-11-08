import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

const gameTextVariants = cva('m-0 uppercase', {
  variants: {
    face: {
      /** Press Start 2P: titles, names, figures of a game screen. */
      pixel: 'font-pixel text-base leading-4',
      /** Silkscreen: subtitles, hints, dates. */
      small: 'font-small text-base leading-4',
    },
    outlined: {
      /** Outlined in ink, for text laid straight over the stars. */
      true: 'text-outline-ink',
      false: '',
    },
  },
  defaultVariants: { face: 'pixel', outlined: false },
});

export type GameTextProps = React.ComponentProps<'p'> &
  VariantProps<typeof gameTextVariants> & {
    as?: 'p' | 'span';
  };

/** A line of a game screen, in the faces and sizes of the client at ×2. */
export function GameText({
  as: Element = 'p',
  face,
  outlined,
  className,
  ...props
}: GameTextProps) {
  return (
    <Element
      data-slot="game-text"
      className={cn(gameTextVariants({ face, outlined }), className)}
      {...props}
    />
  );
}
