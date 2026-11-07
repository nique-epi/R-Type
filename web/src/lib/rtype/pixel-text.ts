/**
 * Removes the accents of a text drawn in Press Start 2P, which has no glyph
 * for them: "Épitech" is drawn "Epitech", as the client draws it.
 */
export function withoutAccents(text: string): string {
  return text.normalize('NFD').replace(/\p{Diacritic}/gu, '');
}
