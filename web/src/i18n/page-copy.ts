import type { frPages } from './dictionaries/fr-pages';

/** A literal type of the copy widened to the type of its words, down to the arrays. */
type Widen<T> = T extends string
  ? string
  : T extends readonly (infer Item)[]
    ? readonly Widen<Item>[]
    : T extends object
      ? { [Key in keyof T]: Widen<T[Key]> }
      : T;

/**
 * The words of the pages in a language, the shape of the French copy: a
 * language that misses a line, or adds one, fails to compile.
 */
export type PageCopy = Widen<typeof frPages>;
