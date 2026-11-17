import { env } from '@/config/server-env';

/** The header the site names itself to the master in, with `MASTER_SITE_KEY`. */
export const SITE_KEY_HEADER = 'X-Site-Key';

/** The header the site names the visitor it acts for in. */
export const SITE_CLIENT_ADDRESS_HEADER = 'X-Site-Client-Address';

/**
 * Who the site acts for: its key and the address of the visitor, so the
 * master counts the quotas of that visitor and not the ones of the site.
 * Nothing without both: the master then counts the address it sees.
 */
export function visitorHeadersOf(
  visitorAddress: string | null,
): Record<string, string> {
  if (!env.MASTER_SITE_KEY || !visitorAddress) {
    return {};
  }

  return {
    [SITE_KEY_HEADER]: env.MASTER_SITE_KEY,
    [SITE_CLIENT_ADDRESS_HEADER]: visitorAddress,
  };
}
