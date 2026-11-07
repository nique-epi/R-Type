import { describe, expect, it } from 'vitest';

import { en } from './en';
import { fr } from './fr';

type Tree = string | readonly Tree[] | { readonly [key: string]: Tree };

/** Every path of a dictionary down to its words, with the words at its end. */
function wordsOf(tree: Tree, path = ''): [string, string][] {
  if (typeof tree === 'string') {
    return [[path, tree]];
  }
  const entries: [string, Tree][] = Array.isArray(tree)
    ? tree.map((item, index) => [String(index), item])
    : Object.entries(tree);

  return entries.flatMap(([key, value]) => wordsOf(value, `${path}/${key}`));
}

function holesOf(words: string): string[] {
  return [...words.matchAll(/\{(\w+)\}/g)].map(match => match[1] ?? '').sort();
}

const FRENCH = wordsOf(fr as unknown as Tree);
const ENGLISH = wordsOf(en as unknown as Tree);

describe('the dictionaries', () => {
  it('say the same things in both languages, line for line', () => {
    expect(ENGLISH.map(([path]) => path)).toEqual(FRENCH.map(([path]) => path));
  });

  it('fill the same holes in both languages', () => {
    const english = new Map(ENGLISH);

    for (const [path, words] of FRENCH) {
      if (path === '/locale') {
        continue;
      }
      expect(holesOf(english.get(path) ?? ''), path).toEqual(holesOf(words));
    }
  });

  it('leave no line empty', () => {
    for (const [path, words] of [...FRENCH, ...ENGLISH]) {
      expect(words.length, path).toBeGreaterThan(0);
    }
  });
});
