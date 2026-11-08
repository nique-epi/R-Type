/**
 * The items of the hangar, in the order of the locker of the game
 * (SHIP_LOOK_ITEM_NAMES, src/identifiers/ShipLookConstants.hpp).
 */
export const LOOK_ITEMS = [
  'topHat',
  'partyHat',
  'crown',
  'propellerCap',
  'cowboyHat',
  'vikingHelmet',
  'wizardHat',
  'chefToque',
  'coolShades',
  'mustache',
] as const;

export type LookItem = (typeof LOOK_ITEMS)[number];

/** The items of a first launch: the starter kit of the locker (ShipLockerConstants.hpp). */
export const STARTER_LOOK_ITEMS: readonly LookItem[] = [
  'topHat',
  'coolShades',
  'mustache',
];

/** The items only a normal or hard game brings (look_items of assets/waves/waves.toml). */
export const HARDER_LOOK_ITEMS: readonly LookItem[] = [
  'vikingHelmet',
  'wizardHat',
  'chefToque',
];

/** How a pilot gets an item: in the starter kit, in any mission, or in a normal or hard one. */
export type LookItemSource = 'starter' | 'mission' | 'harderMission';

export function lookItemSourceOf(item: LookItem): LookItemSource {
  if (STARTER_LOOK_ITEMS.includes(item)) {
    return 'starter';
  }
  return HARDER_LOOK_ITEMS.includes(item) ? 'harderMission' : 'mission';
}
