import type { PlayerNumber } from '@/lib/rtype/players';
import { SHEETS, shipFrame } from './sheets';
import { Sprite, type SpriteProps } from './sprite';

export type ShipProps = Omit<SpriteProps, 'sheet' | 'frame' | 'scale'> & {
  player: PlayerNumber;
  /** 1 is 32 × 16 on screen, 2 is 64 × 32 (the default), 4 is 128 × 64, 8 is 256 × 128. */
  scale?: number;
};

/** The ship of a pilot, in the color of their seat. */
export function Ship({ player, scale = 2, ...props }: ShipProps) {
  return (
    <Sprite
      sheet={SHEETS.entities}
      frame={shipFrame(player)}
      scale={scale}
      data-player={player}
      {...props}
    />
  );
}
