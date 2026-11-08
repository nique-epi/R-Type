import { z } from 'zod';

import { SYSTEMS, type System } from './systems';

/** The file a player installs the client from, on each system. */
const CLIENT_FILE_PATTERNS: Record<System, RegExp> = {
  linux: /^R-Type-client-.+-Linux\.AppImage$/,
  windows: /^R-Type-client-.+-win64\.exe$/,
  macos: /^R-Type-client-.+-Darwin\.dmg$/,
};

/** How many of the newest entries of the release notes the pages show. */
const SHOWN_NOTES = 3;

const releaseSchema = z.object({
  tag_name: z.string().min(1),
  published_at: z.string().min(1),
  body: z.string().nullable(),
  assets: z.array(
    z.object({
      name: z.string(),
      size: z.number(),
      browser_download_url: z.url(),
    }),
  ),
});

/** A file of a release. */
export type ReleaseFile = {
  name: string;
  /** In bytes. */
  size: number;
  url: string;
};

/** What the pages know of the latest release of the repository. */
export type LatestRelease = {
  /** "0.1.0": the tag without its `v`. */
  version: string;
  /** An ISO 8601 date. */
  publishedAt: string;
  /** The newest entries of the release notes, oldest of them first. */
  notes: string[];
  /** The client installer of each system; none when the release has no such file. */
  clients: Record<System, ReleaseFile | null>;
  /** Every other file: the archives of the client and the packages of the server. */
  others: ReleaseFile[];
};

/** The tag of a release, without the `v` before its number. */
function versionOf(tag: string): string {
  return tag.replace(/^v/, '');
}

/**
 * The entries of the notes GitHub generates ("* title by @author in
 * <link>"), without the author and the link.
 */
export function notesOf(body: string | null): string[] {
  const entries = (body ?? '')
    .split('\n')
    .filter(line => line.startsWith('* '))
    .map(line =>
      line
        .slice(2)
        .replace(/ by @\S+ in https?:\/\/\S+$/, '')
        .trim(),
    )
    .filter(entry => entry !== '');

  return entries.slice(-SHOWN_NOTES);
}

/** Reads the answer of `GET /repos/{repository}/releases/latest`; nothing when it is not one. */
export function releaseOf(json: unknown): LatestRelease | null {
  const parsed = releaseSchema.safeParse(json);

  if (!parsed.success) {
    return null;
  }

  const files: ReleaseFile[] = parsed.data.assets.map(asset => ({
    name: asset.name,
    size: asset.size,
    url: asset.browser_download_url,
  }));
  const clients = Object.fromEntries(
    SYSTEMS.map(system => [
      system,
      files.find(file => CLIENT_FILE_PATTERNS[system].test(file.name)) ?? null,
    ]),
  ) as Record<System, ReleaseFile | null>;
  const installers = new Set(Object.values(clients));

  return {
    version: versionOf(parsed.data.tag_name),
    publishedAt: parsed.data.published_at,
    notes: notesOf(parsed.data.body),
    clients,
    others: files.filter(file => !installers.has(file)),
  };
}
