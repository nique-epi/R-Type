import type { PlayerNumber } from '@/lib/rtype/players';

/** A sprite sheet of the game, served from public/sprites/, in its own pixels. */
export type SpriteSheet = {
  src: string;
  width: number;
  height: number;
};

/** One frame of a sheet, in pixels of the sheet. */
export type SpriteFrame = {
  x: number;
  y: number;
  width: number;
  height: number;
};

/**
 * The sheets the site draws, copied from assets/sprites/ of the game; their
 * sizes are the sizes of the files.
 */
export const SHEETS = {
  entities: { src: '/sprites/entities.png', width: 66, height: 140 },
  voltjelly: { src: '/sprites/mobs/voltjelly.png', width: 166, height: 40 },
  reaper: { src: '/sprites/mobs/reaper.png', width: 82, height: 117 },
  drillback: { src: '/sprites/mobs/drillback.png', width: 102, height: 102 },
  bydomancer: { src: '/sprites/mobs/bydomancer.png', width: 127, height: 163 },
  professor: { src: '/sprites/professor.png', width: 50, height: 58 },
} as const satisfies Record<string, SpriteSheet>;

/** The rows of the ships in entities.png: 16 pixels tall, 18 apart. */
const SHIP_ROW_STEP = 18;

/** The ship of a pilot, in the color of their seat. */
export function shipFrame(player: PlayerNumber): SpriteFrame {
  return { x: 0, y: (player - 1) * SHIP_ROW_STEP, width: 32, height: 16 };
}

/** The enemies of version 0.1.0, as the wiki names them. */
export const ENEMY_KINDS = [
  'bydo',
  'voltjelly',
  'reaper',
  'drillback',
  'bydomancer',
] as const;

export type EnemyKind = (typeof ENEMY_KINDS)[number];

/** The sheet and the first frame of each enemy. */
export const ENEMY_SPRITES: Record<
  EnemyKind,
  { sheet: SpriteSheet; frame: SpriteFrame }
> = {
  bydo: {
    sheet: SHEETS.entities,
    frame: { x: 0, y: 72, width: 24, height: 24 },
  },
  voltjelly: {
    sheet: SHEETS.voltjelly,
    frame: { x: 0, y: 0, width: 40, height: 40 },
  },
  reaper: {
    sheet: SHEETS.reaper,
    frame: { x: 0, y: 0, width: 24, height: 24 },
  },
  drillback: {
    sheet: SHEETS.drillback,
    frame: { x: 0, y: 0, width: 24, height: 24 },
  },
  bydomancer: {
    sheet: SHEETS.bydomancer,
    frame: { x: 0, y: 0, width: 24, height: 24 },
  },
};

/** The professor of the help screens, first frame. */
export const PROFESSOR_FRAME: SpriteFrame = {
  x: 0,
  y: 0,
  width: 25,
  height: 29,
};
