/** The headers that name the session of `token` to the master. */
export function bearerHeadersOf(token: string): Record<string, string> {
  return { Authorization: `Bearer ${token}` };
}
