import type * as React from 'react';
import type { PlayerNumber } from '@/lib/rtype/players';
import type { LookItem } from '@/lib/rtype/ship-looks';
import { cn } from '@/lib/utils';
import { LOOK_HEADROOM, lookItemSprite, SHEETS } from './sheets';
import { Ship } from './ship';
import { Sprite } from './sprite';

export type DressedShipProps = React.ComponentProps<'span'> & {
  player: PlayerNumber;
  /** What the ship wears, drawn where the client draws it on the hull. */
  item: LookItem;
  scale: number;
  /** Names the ship and its item for a screen reader; without it, it is decoration. */
  label?: string;
  /** Props of the item, such as a class an animation reads. */
  itemProps?: React.ComponentProps<'span'>;
};

/**
 * The ship of a pilot wearing an item of the hangar, with room above it for
 * the tallest hat, so ships in a row keep their hulls on one line.
 */
export function DressedShip({
  player,
  item,
  scale,
  label,
  itemProps,
  className,
  style,
  ...props
}: DressedShipProps) {
  const { frame, mount } = lookItemSprite(item);
  return (
    <span
      data-slot="dressed-ship"
      className={cn('relative inline-block', className)}
      style={{ paddingTop: LOOK_HEADROOM * scale, ...style }}
      {...(label
        ? { role: 'img', 'aria-label': label }
        : { 'aria-hidden': true })}
      {...props}
    >
      <Ship player={player} scale={scale} className="block" />
      <Sprite
        sheet={SHEETS.looks}
        frame={frame}
        scale={scale}
        {...itemProps}
        className={cn('absolute', itemProps?.className)}
        style={{
          left: mount.x * scale,
          top: (LOOK_HEADROOM + mount.y) * scale,
          ...itemProps?.style,
        }}
      />
    </span>
  );
}
