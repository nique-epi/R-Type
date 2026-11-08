import type { PlayerNumber } from '@/lib/rtype/players';
import { LOOK_ITEMS, type LookItem } from '@/lib/rtype/ship-looks';

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
  bydoTyrant: {
    src: '/sprites/mobs/bydo_tyrant.png',
    width: 518,
    height: 1959,
  },
  professor: { src: '/sprites/professor.png', width: 50, height: 58 },
  flag: { src: '/sprites/flag.png', width: 1664, height: 99 },
  guard: { src: '/sprites/guard.png', width: 112, height: 30 },
  looks: { src: '/sprites/looks.png', width: 178, height: 16 },
} as const satisfies Record<string, SpriteSheet>;

/** The rows of the ships in entities.png: 16 pixels tall, 18 apart. */
const SHIP_ROW_STEP = 18;

/** The ship of a pilot, in the color of their seat. */
export function shipFrame(player: PlayerNumber): SpriteFrame {
  return { x: 0, y: (player - 1) * SHIP_ROW_STEP, width: 32, height: 16 };
}

/** The enemies of the game, the boss last, as the wiki names them. */
export const ENEMY_KINDS = [
  'bydo',
  'voltjelly',
  'reaper',
  'drillback',
  'bydomancer',
  'bydoTyrant',
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
  bydoTyrant: {
    sheet: SHEETS.bydoTyrant,
    frame: { x: 0, y: 0, width: 128, height: 128 },
  },
};

/**
 * The whole scale that draws an enemy at most `pixels` wide: a sprite of
 * 24 pixels at 216 is 9, one of 40 pixels is 5.
 */
export function enemyScaleFor(kind: EnemyKind, pixels: number): number {
  return Math.max(1, Math.floor(pixels / ENEMY_SPRITES[kind].frame.width));
}

/** The professor of the help screens, first frame. */
export const PROFESSOR_FRAME: SpriteFrame = {
  x: 0,
  y: 0,
  width: 25,
  height: 29,
};

/** An item on looks.png, and where its top-left pixel sits on the 32 × 16 ship. */
export type LookItemSprite = {
  frame: SpriteFrame;
  mount: { x: number; y: number };
};

/** The cells of looks.png: 16 pixels wide, 18 apart, each item against the top-left corner of its cell. */
const LOOK_CELL_STEP = 18;

/** The size of each item and its place on the ship, copied from SHIP_LOOK_SPRITES (ShipLookSpriteConstants.hpp). */
const LOOK_ITEM_PLACES: Record<
  LookItem,
  { width: number; height: number; x: number; y: number }
> = {
  topHat: { width: 14, height: 10, x: 9, y: -7 },
  partyHat: { width: 12, height: 11, x: 10, y: -8 },
  crown: { width: 12, height: 8, x: 10, y: -5 },
  propellerCap: { width: 12, height: 8, x: 10, y: -5 },
  cowboyHat: { width: 14, height: 8, x: 9, y: -5 },
  vikingHelmet: { width: 14, height: 9, x: 9, y: -6 },
  wizardHat: { width: 12, height: 12, x: 10, y: -9 },
  chefToque: { width: 12, height: 9, x: 10, y: -6 },
  coolShades: { width: 10, height: 3, x: 17, y: 5 },
  mustache: { width: 12, height: 5, x: 20, y: 9 },
};

/** The frame of an item on looks.png and where it sits on the ship. */
export function lookItemSprite(item: LookItem): LookItemSprite {
  const { width, height, x, y } = LOOK_ITEM_PLACES[item];
  return {
    frame: {
      x: LOOK_ITEMS.indexOf(item) * LOOK_CELL_STEP,
      y: 0,
      width,
      height,
    },
    mount: { x, y },
  };
}

/** How far the tallest hat rises over the ship, in pixels of the sheet: the wizard hat. */
export const LOOK_HEADROOM = 9;
