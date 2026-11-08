import type * as React from 'react';
import { Heading } from '@/components/primitives/text/typography';
import { DressedShip } from '@/components/sprites/dressed-ship';
import type { LookItem, LookItemSource } from '@/lib/rtype/ship-looks';
import { cn } from '@/lib/utils';

/** Screen pixels per pixel of the game in a cell of the locker. */
const LOCKER_SCALE = 3;

export type LockerCell = {
  item: LookItem;
  name: string;
  source: LookItemSource;
  /** "Kit de départ", "Gagné en mission"... */
  sourceLabel: React.ReactNode;
};

const SOURCE_CLASSES: Record<LookItemSource, string> = {
  starter: 'border-edge-blue text-pale-blue',
  mission: 'border-star-gold bg-star-gold text-ink',
  harderMission: 'border-warning-orange bg-warning-orange text-ink',
};

/** The items of the hangar, each worn by a ship, with how a pilot gets it. */
export function HangarLocker({
  cells,
  'aria-label': label,
}: {
  cells: readonly LockerCell[];
  'aria-label': string;
}) {
  return (
    <ul
      aria-label={label}
      data-slot="hangar-locker"
      className="m-0 grid list-none grid-cols-[repeat(auto-fill,minmax(208px,1fr))] gap-4 p-0"
    >
      {cells.map(cell => (
        <li
          key={cell.item}
          className="grid justify-items-center gap-3 rounded-[24px] border-2 border-deep-blue bg-ink px-3 py-5 text-center"
        >
          <DressedShip player={1} item={cell.item} scale={LOCKER_SCALE} />
          <Heading as="h3" size="card">
            {cell.name}
          </Heading>
          <span
            className={cn(
              'inline-block rounded-full border-2 px-3 py-1 small-caps',
              SOURCE_CLASSES[cell.source],
            )}
          >
            {cell.sourceLabel}
          </span>
        </li>
      ))}
    </ul>
  );
}
