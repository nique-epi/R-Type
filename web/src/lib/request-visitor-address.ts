import 'server-only';

import { headers } from 'next/headers';

import { visitorAddressOf } from './visitor-address';

/** The header the host of the site names the visitor of a request in. */
const FORWARDED_FOR_HEADER = 'x-forwarded-for';

/** The address of the visitor of the request being answered, `null` without one. */
export async function requestVisitorAddress(): Promise<string | null> {
  return visitorAddressOf((await headers()).get(FORWARDED_FOR_HEADER));
}
