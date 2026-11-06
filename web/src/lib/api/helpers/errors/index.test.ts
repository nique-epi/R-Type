import { ValidationError } from '@better-fetch/fetch';
import { describe, expect, it } from 'vitest';

import { MASTER_ERROR_CODES } from '../../clients';
import { handleApiError } from '.';

function problem(status: number) {
  return { status, title: 'Problem', statusText: 'Problem' };
}

describe('handleApiError', () => {
  describe.each(Object.entries(MASTER_ERROR_CODES))(
    'HTTP status %s with a problem document',
    (status, code) => {
      it(`maps to ${code}`, () => {
        expect(handleApiError(problem(Number(status)))).toEqual({
          error: code,
          data: null,
        });
      });
    },
  );

  it('maps an error status without a problem document to ERR_API_FETCH_FAILED', () => {
    expect(handleApiError({ status: 404, statusText: 'Not Found' })).toEqual({
      error: 'ERR_API_FETCH_FAILED',
      data: null,
    });
  });

  it('maps a validation error to ERR_API_VALIDATION_FAILED', () => {
    const error = { error: new ValidationError([{ message: 'invalid' }]) };

    expect(handleApiError(error).error).toBe('ERR_API_VALIDATION_FAILED');
  });

  it('maps a network error to ERR_API_FETCH_FAILED', () => {
    const error = { error: new TypeError('fetch failed') };

    expect(handleApiError(error).error).toBe('ERR_API_FETCH_FAILED');
  });

  it.each([
    ['an unmapped status', { status: 418, title: 'Teapot' }],
    ['a missing status', {}],
    ['null', null],
    ['a string', 'boom'],
  ])('maps %s to UNKNOWN_ERROR', (_label, error) => {
    expect(handleApiError(error).error).toBe('UNKNOWN_ERROR');
  });
});
