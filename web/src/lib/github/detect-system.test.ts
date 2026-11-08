import { describe, expect, it } from 'vitest';

import { detectSystem } from './detect-system';

describe('detectSystem', () => {
  it.each([
    [
      'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 Chrome/130.0 Safari/537.36',
      'windows',
    ],
    [
      'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 Safari/605.1.15',
      'macos',
    ],
    [
      'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 Chrome/130.0 Safari/537.36',
      'linux',
    ],
  ] as const)('reads %s as %s', (userAgent, system) => {
    expect(detectSystem(userAgent)).toBe(system);
  });

  it('reads an Android phone, which names Linux, as Linux', () => {
    expect(detectSystem('Mozilla/5.0 (Linux; Android 14; Pixel 8)')).toBe(
      'linux',
    );
  });

  it('falls back to Linux for an agent that names no system', () => {
    expect(detectSystem('')).toBe('linux');
  });
});
