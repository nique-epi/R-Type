import 'server-only';

import { cache } from 'react';
import { fetchLatestRelease } from '@/lib/github/latest-release';

/** Asks GitHub for the latest release once per request of the page. */
export const getLatestRelease = cache(fetchLatestRelease);
