import { describe, expect, it } from 'vitest';

import {
  linkDecisionOutcomeOf,
  typedUserCodeOf,
  userCodeCaretOf,
  userCodeOf,
} from './device-link';

describe('userCodeCaretOf', () => {
  it.each([
    ['after a letter typed in the first group', 'kMH-CXCZ', 1, 'KMHC-XCZ', 1],
    ['after a letter typed past the dash', 'KQMHC-XCZ', 2, 'KQMH-CXCZ', 2],
    [
      'after a letter typed in the second group',
      'XCMH-CqXCZ',
      7,
      'XCMH-CQXC',
      7,
    ],
    ['past the dash put in at the end', 'xcmh', 4, 'XCMH-', 5],
    ['at the end once the dash is erased', 'XCMH', 4, 'XCMH', 4],
    ['at the start', 'XCMH-CXCZ', 0, 'XCMH-CXCZ', 0],
  ])('puts the caret %s', (_label, typed, caret, shown, expected) => {
    expect(userCodeCaretOf(typed, caret, shown)).toBe(expected);
  });
});

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
