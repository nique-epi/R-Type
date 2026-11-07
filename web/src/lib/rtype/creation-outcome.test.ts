import { describe, expect, it } from 'vitest';

import { creationOutcomeOf } from './creation-outcome';

describe('creationOutcomeOf', () => {
  it('gives the server and its key once the master created it', () => {
    expect(
      creationOutcomeOf({
        error: null,
        data: { id: 7, name: 'Paris 1', key: 'rtk_abc' },
      }),
    ).toEqual({ status: 'created', id: 7, name: 'Paris 1', key: 'rtk_abc' });
  });

  it('tells the quota apart from the other refusals', () => {
    expect(
      creationOutcomeOf({ error: 'TOO_MANY_REQUESTS', data: null }),
    ).toEqual({ status: 'quota' });
  });

  it('tells a master that does not answer apart from one that refuses', () => {
    expect(
      creationOutcomeOf({ error: 'ERR_API_FETCH_FAILED', data: null }),
    ).toEqual({ status: 'unreachable' });
    expect(creationOutcomeOf({ error: 'BAD_REQUEST', data: null })).toEqual({
      status: 'refused',
    });
    expect(
      creationOutcomeOf({ error: 'ERR_API_VALIDATION_FAILED', data: null }),
    ).toEqual({ status: 'refused' });
  });
});
