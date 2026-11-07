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
});
