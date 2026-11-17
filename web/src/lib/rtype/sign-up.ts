import { z } from 'zod';

/**
 * The longest nickname the master takes, in bytes of UTF-8
 * (`MAXIMUM_NICKNAME_SIZE` of the protocol): 16 letters without accent.
 */
export const NICKNAME_MAXIMUM_BYTES = 16;

/**
 * The shortest and the longest password the master takes, in bytes of UTF-8
 * (`MINIMUM_PASSWORD_SIZE` and `MAXIMUM_PASSWORD_SIZE` of the master).
 */
export const PASSWORD_MINIMUM_BYTES = 8;
export const PASSWORD_MAXIMUM_BYTES = 128;

/**
 * Why a sign-up is refused before it reaches the master: each is the message
 * of a check of the schemas below, which the form turns into a sentence.
 */
export type SignUpProblem =
  | 'nicknameRequired'
  | 'nicknameTooLong'
  | 'nicknameUnprintable'
  | 'passwordTooShort'
  | 'passwordTooLong'
  | 'passwordUnprintable'
  | 'confirmationDifferent';

const encoder = new TextEncoder();

/** The size of a text as the master counts it: its bytes of UTF-8. */
export function textSizeOf(text: string): number {
  return encoder.encode(text).length;
}

/** The first code a printable character has, and DEL, which is not one. */
const SPACE_CODE = 0x20;
const DELETE_CODE = 0x7f;

/** Whether the master refuses @p character as a control character. */
function isControlCharacter(character: string): boolean {
  const code = character.codePointAt(0) ?? 0;
  return code < SPACE_CODE || code === DELETE_CODE;
}

/** Whether the master reads @p text as printable UTF-8. */
function isPrintable(text: string): boolean {
  return text.isWellFormed() && ![...text].some(isControlCharacter);
}

/**
 * A nickname as the master takes it, the blanks around it left out: 1 to 16
 * bytes of printable UTF-8.
 */
const nicknameSchema = z
  .string()
  .trim()
  .min(1, { error: 'nicknameRequired' satisfies SignUpProblem })
  .refine(nickname => textSizeOf(nickname) <= NICKNAME_MAXIMUM_BYTES, {
    error: 'nicknameTooLong' satisfies SignUpProblem,
  })
  .refine(isPrintable, {
    error: 'nicknameUnprintable' satisfies SignUpProblem,
  });

/**
 * A password as the master takes it, kept as typed, blanks included: 8 to 128
 * bytes of printable UTF-8.
 */
const passwordSchema = z
  .string()
  .refine(password => textSizeOf(password) >= PASSWORD_MINIMUM_BYTES, {
    error: 'passwordTooShort' satisfies SignUpProblem,
  })
  .refine(password => textSizeOf(password) <= PASSWORD_MAXIMUM_BYTES, {
    error: 'passwordTooLong' satisfies SignUpProblem,
  })
  .refine(isPrintable, {
    error: 'passwordUnprintable' satisfies SignUpProblem,
  });

/**
 * What the site sends to `POST /api/users`: the server action checks it again,
 * since a browser can call an action with anything.
 */
export const credentialsSchema = z.object({
  nickname: nicknameSchema,
  password: passwordSchema,
});

export type Credentials = z.infer<typeof credentialsSchema>;

/**
 * What the form asks: the credentials and the password typed a second time,
 * since an account has no e-mail and a password mistyped once is lost.
 */
export const signUpFormSchema = credentialsSchema
  .extend({ confirmation: z.string() })
  .refine(form => form.confirmation === form.password, {
    error: 'confirmationDifferent' satisfies SignUpProblem,
    path: ['confirmation'],
  });

export type SignUpForm = z.input<typeof signUpFormSchema>;

/** An account the master just created. */
export type CreatedUser = {
  id: number;
  nickname: string;
};

/**
 * How a sign-up ended, as the page reads it: the account, or why there is
 * none. `taken` is the 409 of the master, `quota` its 429, `refused` its 400
 * or a sign-up the action refused, and `unreachable` a master that did not
 * answer or answered something else.
 */
export type SignUpOutcome =
  | { status: 'created'; user: CreatedUser }
  | { status: 'taken' | 'quota' | 'refused' | 'unreachable' };

/**
 * What the sign-up action hands back to the page: the outcome, or the
 * credentials it refused, or the error it did not expect.
 */
export type SignUpActionResult = {
  data?: SignUpOutcome;
  serverError?: string;
  validationErrors?: unknown;
};

/** Creates the account of `credentials`: the server action of the page. */
export type SignUp = (
  credentials: Credentials,
) => Promise<SignUpActionResult | undefined>;

/**
 * The outcome the page shows for what the action handed back: credentials
 * the action refused are `refused`, and an error or no answer `unreachable`.
 */
export function outcomeOf(
  result: SignUpActionResult | undefined,
): SignUpOutcome {
  if (result?.data) {
    return result.data;
  }

  return { status: result?.validationErrors ? 'refused' : 'unreachable' };
}
