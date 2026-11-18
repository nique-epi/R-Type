import 'server-only';

import { cookies } from 'next/headers';

/**
 * The cookie the site keeps the session of a player in: the token the master
 * gave, which the browser sends back to the site alone (`HttpOnly`, never
 * read by a script), over HTTPS only (`Secure`), and on a navigation from
 * another site only for a link (`SameSite=Lax`). The site sends it to the
 * master as Bearer; it never reaches a page.
 */
export const SESSION_COOKIE = 'rtype_session';

/** The longest token the site keeps: a token of the master is 68 characters. */
const SESSION_TOKEN_MAXIMUM_LENGTH = 128;

/** Milliseconds in a second, to read a date of the master. */
const MILLISECONDS_PER_SECOND = 1000;

/** The token of the session of the visitor, `null` when it has none. */
export async function readSessionToken(): Promise<string | null> {
  const token = (await cookies()).get(SESSION_COOKIE)?.value;
  if (!token || token.length > SESSION_TOKEN_MAXIMUM_LENGTH) {
    return null;
  }

  return token;
}

/**
 * Keeps `token` for the visitor until `expiresAt`, the second the master says
 * the session expires (in seconds since 1970).
 */
export async function keepSession(token: string, expiresAt: number) {
  (await cookies()).set(SESSION_COOKIE, token, {
    httpOnly: true,
    secure: true,
    sameSite: 'lax',
    path: '/',
    expires: new Date(expiresAt * MILLISECONDS_PER_SECOND),
  });
}

/** Forgets the session of the visitor. */
export async function forgetSession() {
  (await cookies()).delete(SESSION_COOKIE);
}
