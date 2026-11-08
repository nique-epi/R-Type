import { describe, expect, it } from 'vitest';

import { pathOf, serverPathOf, wikiPathOf } from './site-paths';

describe('pathOf', () => {
  it('writes the home page of a language without a trailing slash', () => {
    expect(pathOf('fr', 'home')).toBe('/fr');
    expect(pathOf('en', 'home')).toBe('/en');
  });

  it('writes the other pages under their language', () => {
    expect(pathOf('en', 'servers')).toBe('/en/servers');
    expect(pathOf('fr', 'ranking')).toBe('/fr/leaderboard');
  });
});

describe('serverPathOf', () => {
  it('writes the page of a server under the list of servers', () => {
    expect(serverPathOf('fr', '7')).toBe('/fr/servers/7');
  });

  it('keeps a slash of an id out of the path', () => {
    expect(serverPathOf('fr', 'a/b')).toBe('/fr/servers/a%2Fb');
  });
});

describe('wikiPathOf', () => {
  it('writes a section of the wiki', () => {
    expect(wikiPathOf('en', 'keys')).toBe('/en/wiki/keys');
  });
});
