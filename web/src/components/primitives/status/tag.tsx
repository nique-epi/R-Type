import { cva } from 'class-variance-authority';
import type * as React from 'react';
import type { GameState } from '@/lib/rtype/servers';
import { cn } from '@/lib/utils';

const tagVariants = cva(
  'inline-block rounded-full border-2 px-3 py-1 small-caps',
  {
    variants: {
      state: {
        waiting: 'border-edge-blue text-pale-blue',
        playing: 'border-edge-blue bg-edge-blue text-ink',
        full: 'border-star-gold bg-star-gold text-ink',
      },
    },
  },
);

export type TagProps = React.ComponentProps<'span'> & {
  state: GameState;
  children: React.ReactNode;
};

/** The state of a game in a word: waiting, playing, full. */
export function Tag({ state, className, ...props }: TagProps) {
  return (
    <span
      data-slot="tag"
      data-state={state}
      className={cn(tagVariants({ state }), className)}
      {...props}
    />
  );
}
