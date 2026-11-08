import { cva, type VariantProps } from 'class-variance-authority';
import { Ship } from '@/components/sprites/ship';
import type { PlayerNumber } from '@/lib/rtype/players';
import { cn } from '@/lib/utils';

/** A seat of a game: the pilot in it, or null when it is free. */
export type Seat = {
  player: PlayerNumber;
  /** Names the pilot for a pointer and a screen reader: "Pilote 2". */
  title: string;
  /** The seat of the reader, drawn with a white edge. */
  isYou?: boolean;
} | null;

const slotVariants = cva(
  'box-border inline-flex items-center justify-center rounded-[10px] border-2',
  {
    variants: {
      size: {
        md: 'h-10 w-18',
        sm: 'h-6 w-10',
      },
      free: {
        true: 'border-blue border-dashed bg-transparent font-small text-base text-pale-blue uppercase',
        false: 'border-edge-blue bg-ink',
      },
    },
    defaultVariants: { size: 'md', free: false },
  },
);

export type PlayerSlotsProps = VariantProps<typeof slotVariants> & {
  seats: readonly Seat[];
  /** The word in a free seat; the small slots keep it for screen readers only. */
  freeLabel: string;
  className?: string;
};

/** One slot per seat of a game: the ship of each pilot, a dashed frame for a free seat. */
export function PlayerSlots({
  seats,
  freeLabel,
  size = 'md',
  className,
}: PlayerSlotsProps) {
  return (
    <ul
      data-slot="player-slots"
      className={cn('m-0 inline-flex list-none gap-2 p-0', className)}
    >
      {seats.map((seat, index) => (
        <li
          // biome-ignore lint/suspicious/noArrayIndexKey: a seat is its place in the game; it has no other identity.
          key={index}
          title={seat?.title}
          className={cn(
            slotVariants({ size, free: seat === null }),
            seat?.isYou && 'border-star-white',
          )}
        >
          {seat === null ? (
            <span className={size === 'sm' ? 'sr-only' : undefined}>
              {freeLabel}
            </span>
          ) : (
            <Ship
              player={seat.player}
              scale={size === 'sm' ? 1 : 2}
              label={seat.title}
            />
          )}
        </li>
      ))}
    </ul>
  );
}
