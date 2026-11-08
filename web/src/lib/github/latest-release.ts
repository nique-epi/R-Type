import 'server-only';

import { env } from '@/config/server-env';

import { type LatestRelease, releaseOf } from './release';

const GITHUB_API = 'https://api.github.com';

/**
 * How long Next.js keeps the answer. Without a token the API answers 60
 * requests an hour to one address: a read every ten minutes stays far below.
 */
const REVALIDATE_SECONDS = 600;

/** How long GitHub may take before the page goes on without it. */
const TIMEOUT_MILLISECONDS = 5000;

/** The page that lists the releases of the repository. */
export function releasesPageUrl(): string {
  return `https://github.com/${env.GITHUB_REPOSITORY}/releases`;
}

function headers(): HeadersInit {
  return {
    Accept: 'application/vnd.github+json',
    ...(env.GITHUB_TOKEN
      ? { Authorization: `Bearer ${env.GITHUB_TOKEN}` }
      : {}),
  };
}

/**
 * Reads the latest release of the repository. Nothing when GitHub does not
 * answer, answers an error (the limit of the API included) or answers
 * something that is not a release: the pages then link to the releases page.
 */
export async function fetchLatestRelease(): Promise<LatestRelease | null> {
  try {
    const response = await fetch(
      `${GITHUB_API}/repos/${env.GITHUB_REPOSITORY}/releases/latest`,
      {
        headers: headers(),
        next: { revalidate: REVALIDATE_SECONDS },
        signal: AbortSignal.timeout(TIMEOUT_MILLISECONDS),
      },
    );

    return response.ok ? releaseOf(await response.json()) : null;
  } catch {
    return null;
  }
}
