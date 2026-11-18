import { describe, expect, it } from 'vitest';

import { nextPathOf, signInFormSchema, signInOutcomeOf } from './sign-in';

const HOME = '/fr';

describe('nextPathOf', () => {
  it('leads to a page of the site, with its query', () => {
    expect(nextPathOf('/fr/link?code=KQFB-WZPM', HOME)).toBe(
      '/fr/link?code=KQFB-WZPM',
    );
  });

  it.each([
    ['another site', 'https://evil.test/fr'],
    ['a path that names another host', '//evil.test/fr'],
    ['a backslash browsers read as a slash', '/\\evil.test'],
    ['a relative path', 'fr/link'],
    ['nothing', undefined],
  ])('leads home instead of %s', (_label, next) => {
    expect(nextPathOf(next, HOME)).toBe(HOME);
  });
});

describe('signInFormSchema', () => {
  it('keeps the nickname without the blanks around it', () => {
    expect(
      signInFormSchema.parse({ nickname: ' ACE ', password: 'secret' }),
    ).toEqual({ nickname: 'ACE', password: 'secret' });
  });

  it('refuses an empty password', () => {
    expect(
      signInFormSchema.safeParse({ nickname: 'ACE', password: '' }).success,
    ).toBe(false);
  });
});

describe('signInOutcomeOf', () => {
  it('reads no answer as an unreachable master', () => {
    expect(signInOutcomeOf(undefined)).toEqual({ status: 'unreachable' });
  });
});
