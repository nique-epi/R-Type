import { describe, expect, it } from 'vitest';

import { linkDecisionOutcomeOf, userCodeOf } from './device-link';

describe('userCodeOf', () => {
  it.each([
    ['as the master keeps it', 'KQFB-WZPM'],
    ['in lowercase', 'kqfb-wzpm'],
    ['without its dash', 'KQFBWZPM'],
    ['with blanks around', '  kqfbwzpm '],
  ])('reads a code typed %s', (_label, typed) => {
    expect(userCodeOf(typed)).toBe('KQFB-WZPM');
  });

  it.each([
    ['a vowel', 'KQFB-WZPA'],
    ['a digit', 'KQFB-WZP1'],
    ['a letter short', 'KQFB-WZP'],
    ['a letter too many', 'KQFB-WZPMB'],
    ['nothing', ''],
    ['no text', undefined],
  ])('refuses a code with %s', (_label, typed) => {
    expect(userCodeOf(typed)).toBeNull();
  });
});

describe('linkDecisionOutcomeOf', () => {
  it('returns the outcome the action gave', () => {
    expect(linkDecisionOutcomeOf({ data: { status: 'approved' } })).toEqual({
      status: 'approved',
    });
  });

  it('reads a decision the action refused as an unknown code', () => {
    expect(linkDecisionOutcomeOf({ validationErrors: {} })).toEqual({
      status: 'unknown',
    });
  });

  it('reads no answer as an unreachable master', () => {
    expect(linkDecisionOutcomeOf(undefined)).toEqual({ status: 'unreachable' });
  });
});
