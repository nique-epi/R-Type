import { describe, expect, it } from 'vitest';

import { masterHealthOf } from './health.hook';

describe('masterHealthOf', () => {
  it('reads a master that answers as online, with its version', () => {
    expect(masterHealthOf({ data: { version: '0.1.0' }, error: null })).toEqual(
      { state: 'online', version: '0.1.0' },
    );
  });

  it('reads any error as an unreachable master', () => {
    expect(
      masterHealthOf({ data: null, error: 'ERR_API_FETCH_FAILED' }),
    ).toEqual({ state: 'unreachable' });
  });
});
