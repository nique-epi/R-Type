/** The colors of pilots 1 to 4 in one palette of the accessibility options. */
export type PaletteColors = readonly [string, string, string, string];

/**
 * The palettes the client offers (`Accessibility` options of the game):
 * pilot 1 always keeps the blue. The names are words of a language, so they
 * live in the dictionaries under the same ids.
 */
export const PALETTE_IDS = [
  'standard',
  'protanopia',
  'deuteranopia',
  'tritanopia',
] as const;

export type PaletteId = (typeof PALETTE_IDS)[number];

export const PALETTE_COLORS: Record<PaletteId, PaletteColors> = {
  standard: ['#1fa3c9', '#d94a8c', '#7ec23a', '#e09a2a'],
  protanopia: ['#1fa3c9', '#ee3377', '#009988', '#f0e442'],
  deuteranopia: ['#1fa3c9', '#cc79a7', '#009e73', '#f0e442'],
  tritanopia: ['#1fa3c9', '#228833', '#7ec23a', '#f0e442'],
};
