import { ENEMY_SPRITES, type EnemyKind } from './sheets';
import { Sprite, type SpriteProps } from './sprite';

export type EnemyProps = Omit<SpriteProps, 'sheet' | 'frame' | 'scale'> & {
  kind: EnemyKind;
  /** 3 by default: 72 × 72 for a Bydo, 120 × 120 for a Voltjelly. */
  scale?: number;
};

/** A Bydo of the game, by its kind. */
export function Enemy({ kind, scale = 3, ...props }: EnemyProps) {
  const sprite = ENEMY_SPRITES[kind];
  return (
    <Sprite
      sheet={sprite.sheet}
      frame={sprite.frame}
      scale={scale}
      data-enemy={kind}
      {...props}
    />
  );
}
