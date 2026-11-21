import { describe, expect, it } from 'vitest';

import {
  linkDecisionOutcomeOf,
  typedUserCodeOf,
  userCodeOf,
} from './device-link';

describe('typedUserCodeOf', () => {
  it.each([
    ['three letters', 'kqf', 'kq', 'KQF'],
    ['the fourth letter, with the dash after it', 'kqfb', 'KQF', 'KQFB-'],
    ['a fifth letter after the dash', 'KQFB-w', 'KQFB-', 'KQFB-W'],
    ['a dash of its own after the one put in', 'KQFB--', 'KQFB-', 'KQFB-'],
    ['the whole code pasted', ' kqfb wzpm ', '', 'KQFB-WZPM'],
    ['a letter past the last', 'KQFB-WZPMB', 'KQFB-WZPM', 'KQFB-WZPM'],
    ['a vowel and a digit, kept for the check', 'ka1', 'ka', 'KA1'],
  ])('writes %s', (_label, typed, previous, shown) => {
    expect(typedUserCodeOf(typed, previous)).toBe(shown);
  });

  it.each([
    ['the dash', 'KQFB', 'KQFB-', 'KQFB'],
    ['the letter after the dash', 'KQFB-', 'KQFB-W', 'KQFB'],
    ['the fourth letter', 'KQF', 'KQFB', 'KQF'],
  ])('lets the player erase %s', (_label, typed, previous, shown) => {
    expect(typedUserCodeOf(typed, previous)).toBe(shown);
  });
});

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
