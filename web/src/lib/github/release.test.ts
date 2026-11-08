import { describe, expect, it } from 'vitest';

import { notesOf, releaseOf } from './release';
import { RELEASE_V0_1_0 } from './release.fixture';

const MEBIBYTE_BYTES = 1048576;

describe('releaseOf', () => {
  it('names the version after the tag, without its v', () => {
    expect(releaseOf(RELEASE_V0_1_0)?.version).toBe('0.1.0');
  });

  it('keeps the second the release was published', () => {
    expect(releaseOf(RELEASE_V0_1_0)?.publishedAt).toBe('2026-11-06T21:31:56Z');
  });

  it('picks the installer of each system', () => {
    const clients = releaseOf(RELEASE_V0_1_0)?.clients;

    expect(clients?.linux?.name).toBe('R-Type-client-0.1.0-Linux.AppImage');
    expect(clients?.windows?.name).toBe('R-Type-client-0.1.0-win64.exe');
    expect(clients?.macos?.name).toBe('R-Type-client-0.1.0-Darwin.dmg');
  });

  it('gives the size and the address of the file as GitHub lists them', () => {
    const linux = releaseOf(RELEASE_V0_1_0)?.clients.linux;

    expect(linux?.size).toBe(15727096);
    expect(linux?.size).toBeGreaterThan(14 * MEBIBYTE_BYTES);
    expect(linux?.url).toBe(
      'https://github.com/nique-epi/R-Type/releases/download/v0.1.0/R-Type-client-0.1.0-Linux.AppImage',
    );
  });

  it('leaves the archives and the server packages to the other files', () => {
    expect(releaseOf(RELEASE_V0_1_0)?.others.map(file => file.name)).toEqual([
      'R-Type-client-0.1.0-Linux.tar.gz',
      'R-Type-client-0.1.0-win64.zip',
      'R-Type-server-0.1.0-Darwin.tar.gz',
      'R-Type-server-0.1.0-Linux.tar.gz',
      'R-Type-server-0.1.0-win64.exe',
      'R-Type-server-0.1.0-win64.zip',
    ]);
  });

  it('has no installer for a system the release has no file for', () => {
    const withoutMac = {
      ...RELEASE_V0_1_0,
      assets: RELEASE_V0_1_0.assets.filter(
        asset => !asset.name.endsWith('.dmg'),
      ),
    };

    expect(releaseOf(withoutMac)?.clients.macos).toBeNull();
  });

  it('is nothing when the answer is not a release', () => {
    expect(releaseOf({ message: 'API rate limit exceeded' })).toBeNull();
  });
});

describe('notesOf', () => {
  it('drops the author and the link of each entry', () => {
    expect(notesOf(RELEASE_V0_1_0.body).at(-1)).toBe(
      'ci: build the packages on a pull request only when it changes the packaging',
    );
  });

  it('keeps the newest three entries, the oldest of them first', () => {
    expect(notesOf(RELEASE_V0_1_0.body)).toHaveLength(3);
    expect(notesOf(RELEASE_V0_1_0.body)[0]).toBe(
      'feat: package the client and the server, and release them on every tag',
    );
  });

  it('has no entry for a release without notes', () => {
    expect(notesOf(null)).toEqual([]);
  });
});
