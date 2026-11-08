import { describe, expect, it } from 'vitest';

import { isCurrent } from './nav-item';

describe('isCurrent', () => {
  it('marks the home link on the home page only', () => {
    expect(isCurrent('/', '/')).toBe(true);
    expect(isCurrent('/', '/servers')).toBe(false);
  });

  it('marks a section on its page and on the pages under it', () => {
    expect(isCurrent('/servers', '/servers')).toBe(true);
    expect(isCurrent('/servers', '/servers/nebula-lyon')).toBe(true);
  });

  it('never marks a section on a page whose name only starts the same', () => {
    expect(isCurrent('/server', '/servers')).toBe(false);
  });

  it('marks the home page of a language on that page only, when it asks to be exact', () => {
    expect(isCurrent('/fr', '/fr', true)).toBe(true);
    expect(isCurrent('/fr', '/fr/servers', true)).toBe(false);
  });

  it('marks a section under a language on the pages under it', () => {
    expect(isCurrent('/fr/servers', '/fr/servers/new')).toBe(true);
    expect(isCurrent('/fr/servers', '/en/servers')).toBe(false);
  });
});
