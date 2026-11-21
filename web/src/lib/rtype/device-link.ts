/**
 * The letters of a short code, the consonants RFC 8628 suggests (section
 * 6.1), how many it holds, and the dash between its two groups: what the
 * master draws (`DEVICE_LINK_USER_CODE_ALPHABET` of the master).
 */
const USER_CODE_ALPHABET = 'BCDFGHJKLMNPQRSTVWXZ';
const USER_CODE_LETTERS = 8;
const USER_CODE_GROUP = 4;
const USER_CODE_SEPARATOR = '-';

/**
 * The short code `text` writes, as the master keeps it: `KQFB-WZPM`. A player
 * may type it in any case, with or without its dash, with blanks around;
 * `null` for any other text, which the page never sends to the master.
 */
export function userCodeOf(text: string | null | undefined): string | null {
  const letters = (text ?? '')
    .trim()
    .toUpperCase()
    .replaceAll(USER_CODE_SEPARATOR, '');
  if (
    letters.length !== USER_CODE_LETTERS ||
    ![...letters].every(letter => USER_CODE_ALPHABET.includes(letter))
  ) {
    return null;
  }

  return `${letters.slice(0, USER_CODE_GROUP)}${USER_CODE_SEPARATOR}${letters.slice(USER_CODE_GROUP)}`;
}

/** Anything a player types that is neither a letter nor a digit. */
const NOT_A_CODE_CHARACTER = /[^A-Z0-9]/g;

/**
 * What the code field shows once a player typed `typed` over `previous`: the
 * letters and digits in capitals, at most as many as a code holds, with the
 * dash put in after the first group. The dash comes as soon as the fourth
 * letter is typed, but not when the player erases back to four letters, so
 * erasing the dash is never undone. A vowel or a digit stays, for
 * `userCodeOf` to refuse.
 */
export function typedUserCodeOf(typed: string, previous: string): string {
  const letters = typed
    .toUpperCase()
    .replaceAll(NOT_A_CODE_CHARACTER, '')
    .slice(0, USER_CODE_LETTERS);
  const growing = typed.length > previous.length;
  if (
    letters.length > USER_CODE_GROUP ||
    (letters.length === USER_CODE_GROUP && growing)
  ) {
    return `${letters.slice(0, USER_CODE_GROUP)}${USER_CODE_SEPARATOR}${letters.slice(USER_CODE_GROUP)}`;
  }

  return letters;
}

/**
 * Where the caret goes in `shown`, the code `typedUserCodeOf` wrote, after the
 * player typed `typed` with the caret at `caret`: after as many letters and
 * digits as stood before it, the dash put in left aside; at the end of `shown`
 * when the caret stood after the last of them, so typing at the end goes on
 * after the dash.
 */
export function userCodeCaretOf(
  typed: string,
  caret: number,
  shown: string,
): number {
  const before = typed
    .slice(0, caret)
    .toUpperCase()
    .replaceAll(NOT_A_CODE_CHARACTER, '').length;
  if (before >= shown.replaceAll(USER_CODE_SEPARATOR, '').length) {
    return shown.length;
  }
  let passed = 0;
  let index = 0;
  while (passed < before) {
    if (shown[index] !== USER_CODE_SEPARATOR) {
      passed += 1;
    }
    index += 1;
  }

  return index;
}

/**
 * How a decision on a link ended, as the page reads it: the link connected or
 * refused, or why the decision was not written. `decided` is the 409 of the
 * master, `unknown` its 404, `signedOut` its 401.
 */
export type LinkDecisionOutcome = {
  status:
    | 'approved'
    | 'denied'
    | 'decided'
    | 'unknown'
    | 'signedOut'
    | 'unreachable';
};

/** What the decision action hands back to the page. */
export type LinkDecisionActionResult = {
  data?: LinkDecisionOutcome;
  serverError?: string;
  validationErrors?: unknown;
};

/** The outcome the page shows for what the action handed back. */
export function linkDecisionOutcomeOf(
  result: LinkDecisionActionResult | undefined,
): LinkDecisionOutcome {
  if (result?.data) {
    return result.data;
  }

  return { status: result?.validationErrors ? 'unknown' : 'unreachable' };
}
