import type * as React from 'react';
import { Keycap } from '@/components/primitives/keycap';
import { cn } from '@/lib/utils';

export type GameChipProps = React.ComponentProps<'button'> & {
  /** The key that does the same thing, printed on the chip: Q, ENTREE, ECHAP. */
  shortcut?: string;
  /** Drawn lit, as the chip the keyboard is on. */
  selected?: boolean;
};

/**
 * A chip of the bottom bar of a game screen: a word and the key that does
 * the same, in a frame with cut corners that lights up in cyan.
 */
export function GameChip({
  shortcut,
  selected = false,
  className,
  children,
  type = 'button',
  ...props
}: GameChipProps) {
  return (
    <button
      type={type}
      data-slot="game-chip"
      data-selected={selected || undefined}
      className={cn(
        'cut-6 relative box-border inline-flex h-8 cursor-pointer items-center gap-3 bg-edge-blue pr-1.5 pl-[22px] text-pale-blue outline-none',
        'lit:bg-hud-cyan lit:text-ink',
        'disabled:cursor-not-allowed disabled:bg-deep-blue disabled:text-blue',
        !shortcut && 'pr-6 pl-6',
        className,
      )}
      {...props}
    >
      <span
        aria-hidden="true"
        className="cut-6 absolute inset-0.5 bg-sky lit-inside:bg-edge-blue"
      />
      <span className="relative font-pixel text-base uppercase leading-4">
        {children}
      </span>
      {shortcut ? (
        <Keycap size="sm" className="relative lit-inside:border-hud-cyan">
          {shortcut}
        </Keycap>
      ) : null}
    </button>
  );
}
