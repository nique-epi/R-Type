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
