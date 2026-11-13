/** What separates two addresses of an `X-Forwarded-For` header. */
const ADDRESS_SEPARATOR = ',';

/**
 * The address of the visitor a request comes from: the last address of its
 * `X-Forwarded-For` header, the one the proxy nearest the site saw, as the
 * master reads its own. Vercel overwrites the header with the public address
 * of the client, so a visitor cannot choose it there. `null` without one.
 */
export function visitorAddressOf(forwardedFor: string | null): string | null {
  const last = forwardedFor?.split(ADDRESS_SEPARATOR).at(-1)?.trim();

  return last ? last : null;
}
