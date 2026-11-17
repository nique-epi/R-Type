import { z } from 'zod';

import {
  NICKNAME_MAXIMUM_BYTES,
  PASSWORD_MAXIMUM_BYTES,
  textSizeOf,
} from './sign-up';

/** Why a sign-in is refused before it reaches the master. */
export type SignInProblem = 'nicknameRequired' | 'passwordRequired';

/**
 * What the sign-in form sends: a nickname and a password, both required and
 * no longer than the master takes. A password is not checked further here,
 * since the master answers a wrong one the same as an unknown nickname.
 */
export const signInFormSchema = z.object({
  nickname: z
    .string()
    .trim()
    .min(1, { error: 'nicknameRequired' satisfies SignInProblem })
    .refine(nickname => textSizeOf(nickname) <= NICKNAME_MAXIMUM_BYTES),
  password: z
    .string()
    .min(1, { error: 'passwordRequired' satisfies SignInProblem })
    .refine(password => textSizeOf(password) <= PASSWORD_MAXIMUM_BYTES),
});

export type SignInForm = z.infer<typeof signInFormSchema>;

/**
 * How a sign-in ended, as the page reads it: signed in, or why not. `wrong`
 * is the 401 of the master, `quota` its 429 after too many failures, and
 * `unreachable` a master that did not answer or answered something else.
 */
export type SignInOutcome =
  | { status: 'signedIn'; nickname: string }
  | { status: 'wrong' | 'quota' | 'unreachable' };

/** What the sign-in action hands back to the page. */
export type SignInActionResult = {
  data?: SignInOutcome;
  serverError?: string;
  validationErrors?: unknown;
};

/** Signs in with `form`: the server action of the page. */
export type SignIn = (
  form: SignInForm,
) => Promise<SignInActionResult | undefined>;

/** The outcome the page shows for what the action handed back. */
export function signInOutcomeOf(
  result: SignInActionResult | undefined,
): SignInOutcome {
  if (result?.data) {
    return result.data;
  }

  return { status: result?.validationErrors ? 'wrong' : 'unreachable' };
}

/** The longest path the sign-in page goes back to after signing in. */
const NEXT_PATH_MAXIMUM_LENGTH = 512;

/**
 * The page the sign-in page leads to once signed in: `next` when it is a path
 * of this site, `fallback` otherwise. A path is one slash then no second
 * slash or backslash, so `//evil.test` or `https://evil.test` never leads off
 * the site (an open redirect).
 */
export function nextPathOf(next: string | null | undefined, fallback: string) {
  if (
    !next ||
    next.length > NEXT_PATH_MAXIMUM_LENGTH ||
    !next.startsWith('/') ||
    next.startsWith('//') ||
    next.includes('\\')
  ) {
    return fallback;
  }

  return next;
}
