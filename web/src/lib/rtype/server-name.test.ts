import { describe, expect, it } from 'vitest';

import { serverNameProblemOf, serverNameSizeOf } from './server-name';

describe('serverNameProblemOf', () => {
  it('refuses a name with no letter', () => {
    expect(serverNameProblemOf('')).toBe('empty');
    expect(serverNameProblemOf('   ')).toBe('empty');
  });

  it('takes a name of 32 bytes and refuses one of 33', () => {
    expect(serverNameProblemOf('a'.repeat(32))).toBeNull();
    expect(serverNameProblemOf('a'.repeat(33))).toBe('tooLong');
  });

  it('leaves the blanks around the name out of its size', () => {
    expect(serverNameProblemOf(`  ${'a'.repeat(32)}  `)).toBeNull();
  });

  it('counts an accented letter as the two bytes the master reads', () => {
    expect(serverNameSizeOf('é')).toBe(2);
    expect(serverNameProblemOf('é'.repeat(16))).toBeNull();
    expect(serverNameProblemOf('é'.repeat(17))).toBe('tooLong');
  });
});
