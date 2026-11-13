import { describe, expect, it } from 'vitest';

import { visitorAddressOf } from './visitor-address';

describe('visitorAddressOf', () => {
  it('reads the only address of the header', () => {
    expect(visitorAddressOf('203.0.113.7')).toBe('203.0.113.7');
  });

  it('keeps the last address, the one the nearest proxy wrote', () => {
    expect(visitorAddressOf('198.51.100.1, 203.0.113.7 ')).toBe('203.0.113.7');
  });

  it('names nobody without a header, or with an empty one', () => {
    expect(visitorAddressOf(null)).toBeNull();
    expect(visitorAddressOf('')).toBeNull();
    expect(visitorAddressOf('203.0.113.7, ')).toBeNull();
  });
});
