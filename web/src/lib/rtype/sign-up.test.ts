import { describe, expect, it } from 'vitest';

import {
  credentialsSchema,
  outcomeOf,
  type SignUpProblem,
  signUpFormSchema,
} from './sign-up';

const PASSWORD = 'correct horse battery staple';

/** The problems the form schema finds in `form`, by field. */
function problemsOf(form: {
  nickname: string;
  password: string;
  confirmation: string;
}): Record<string, SignUpProblem> {
  const result = signUpFormSchema.safeParse(form);
  if (result.success) {
    return {};
  }
  return Object.fromEntries(
    result.error.issues.map(issue => [
      String(issue.path[0]),
      issue.message as SignUpProblem,
    ]),
  );
}

function withNickname(nickname: string) {
  return problemsOf({ nickname, password: PASSWORD, confirmation: PASSWORD });
}

function withPassword(password: string) {
  return problemsOf({ nickname: 'ACE', password, confirmation: password });
}

describe('signUpFormSchema', () => {
  it('takes a nickname and the same password twice', () => {
    expect(withNickname('ACE')).toEqual({});
  });

  it.each([
    ['an empty nickname', ''],
    ['a nickname of blanks', '   '],
  ])('refuses %s', (_case, nickname) => {
    expect(withNickname(nickname)).toEqual({ nickname: 'nicknameRequired' });
  });

  it('takes a nickname of 16 bytes and refuses one of 17', () => {
    expect(withNickname('a'.repeat(16))).toEqual({});
    expect(withNickname('a'.repeat(17))).toEqual({
      nickname: 'nicknameTooLong',
    });
  });

  it('counts an accented letter as two bytes, as the master does', () => {
    expect(withNickname('é'.repeat(8))).toEqual({});
    expect(withNickname(`${'é'.repeat(8)}a`)).toEqual({
      nickname: 'nicknameTooLong',
    });
  });

  it.each([
    ['a tab', 'A\tCE'],
    ['a delete', 'A\u007fCE'],
    ['a lone surrogate', 'A\ud800CE'],
  ])('refuses a nickname holding %s', (_case, nickname) => {
    expect(withNickname(nickname)).toEqual({ nickname: 'nicknameUnprintable' });
  });

  it('takes a password of 8 bytes and refuses one of 7', () => {
    expect(withPassword('a'.repeat(8))).toEqual({});
    expect(withPassword('a'.repeat(7))).toEqual({
      password: 'passwordTooShort',
    });
  });

  it('takes a password of 128 bytes and refuses one of 129', () => {
    expect(withPassword('a'.repeat(128))).toEqual({});
    expect(withPassword('a'.repeat(129))).toEqual({
      password: 'passwordTooLong',
    });
  });

  it('refuses a password holding a line break', () => {
    expect(withPassword('correct\nhorse')).toEqual({
      password: 'passwordUnprintable',
    });
  });

  it('refuses a confirmation that differs from the password', () => {
    expect(
      problemsOf({
        nickname: 'ACE',
        password: PASSWORD,
        confirmation: `${PASSWORD}!`,
      }),
    ).toEqual({ confirmation: 'confirmationDifferent' });
  });
});

describe('credentialsSchema', () => {
  it('trims the nickname and keeps the password as typed', () => {
    expect(
      credentialsSchema.parse({ nickname: '  ACE ', password: '  spaced  ' }),
    ).toEqual({ nickname: 'ACE', password: '  spaced  ' });
  });
});

describe('outcomeOf', () => {
  it('gives the outcome of the action', () => {
    expect(outcomeOf({ data: { status: 'taken' } })).toEqual({
      status: 'taken',
    });
  });

  it('reads credentials the action refused as refused', () => {
    expect(outcomeOf({ validationErrors: { nickname: {} } })).toEqual({
      status: 'refused',
    });
  });

  it.each([
    ['an error of the action', { serverError: 'UNEXPECTED_ERROR' }],
    ['no answer', undefined],
  ])('reads %s as unreachable', (_case, result) => {
    expect(outcomeOf(result)).toEqual({ status: 'unreachable' });
  });
});
