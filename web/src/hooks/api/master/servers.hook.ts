import 'server-only';

import { cache } from 'react';
import { getLatestRelease } from '@/hooks/api/github/latest-release.hook';
import {
  api,
  type ListedServer,
  MAXIMUM_PAGE_SIZE,
  type Sample,
} from '@/lib/api';
import { mapServerOf, type ServerDetails } from '@/lib/rtype/map-server';
import { SAMPLE_SERVERS, sampleSeries } from '@/lib/rtype/sample-servers';
import type { MapServer } from '@/lib/rtype/servers';

/** Where the servers come from: the master, or the examples while it is silent. */
type ServerSource = 'master' | 'sample';

type ServerList = {
  source: ServerSource;
  servers: readonly MapServer[];
};

/**
 * The most servers the pages keep, busiest first: each one costs a read of
 * its games (and of its last sample, for the map), and the map cannot draw
 * more stars than this and stay readable.
 */
const MAXIMUM_LISTED_SERVERS = 30;

/** The busiest servers first, those with no pilot last, then by name. */
function busiestFirst(listed: readonly ListedServer[]): ListedServer[] {
  return [...listed]
    .sort(
      (first, second) =>
        second.playerCount - first.playerCount ||
        first.name.localeCompare(second.name),
    )
    .slice(0, MAXIMUM_LISTED_SERVERS);
}

/**
 * The games of a server, and its ping when `withPing` asks for it (a read of
 * its last sample, which only the map needs); a read that fails leaves it
 * with none.
 */
async function detailsOf(
  server: ListedServer,
  withPing: boolean,
): Promise<ServerDetails> {
  const [games, samples] = await Promise.all([
    api.servers.getGames(server.id),
    withPing ? api.servers.getSamples(server.id, 1) : null,
  ]);

  return {
    games: games.error ? [] : games.data,
    ping: samples?.error
      ? undefined
      : samples?.data[0]?.measures?.averagePingMilliseconds,
  };
}

async function readServers(withPing: boolean): Promise<ServerList> {
  const [listing, release] = await Promise.all([
    api.servers.listServers(),
    getLatestRelease(),
  ]);

  if (listing.error) {
    return { source: 'sample', servers: SAMPLE_SERVERS };
  }

  const servers = await Promise.all(
    busiestFirst(listing.data).map(async server =>
      mapServerOf(server, await detailsOf(server, withPing), release?.version),
    ),
  );

  return { source: 'master', servers };
}

/**
 * Reads the servers of the master with their games. When the master does not
 * answer the list, the examples stand in and `source` says so.
 */
export const getServerList = cache(
  async (): Promise<ServerList> => readServers(false),
);

/** The same servers with their ping, one more read each: the sector map draws them by it. */
export const getMappedServerList = cache(
  async (): Promise<ServerList> => readServers(true),
);

/** The names of the servers, from the list alone: what a filter of the leaderboard offers. */
export const getServerNames = cache(async (): Promise<string[]> => {
  const listing = await api.servers.listServers();

  return (listing.error ? SAMPLE_SERVERS : busiestFirst(listing.data)).map(
    server => server.name,
  );
});

/** Everything the page of one server shows. */
export type ServerSheet = {
  source: ServerSource;
  /** The server; nothing while it is created and has not registered yet. */
  server: MapServer | null;
  /** The pilots at each sample, oldest first. */
  pilots: readonly number[];
  /** Seconds between the oldest sample and the newest; 0 with fewer than two. */
  span: number;
};

/** The pilots of an example server's chart: a day, as the mockup draws it. */
const SAMPLE_CHART_LENGTH = 48;
const SAMPLE_CHART_MAXIMUM = 12;
const SAMPLE_CHART_SPAN_SECONDS = 86400;

type SampleTimes = Pick<Sample, 'takenAt' | 'playerCount'>;

/** The pilots of the samples, which the master lists newest first, oldest first. */
function pilotsOfSamples(samples: readonly SampleTimes[]): number[] {
  return samples.map(sample => sample.playerCount).reverse();
}

function spanOf(samples: readonly SampleTimes[]): number {
  const newest = samples[0];
  const oldest = samples.at(-1);

  return newest && oldest ? newest.takenAt - oldest.takenAt : 0;
}

function sampleSheet(server: MapServer): ServerSheet {
  return {
    source: 'sample',
    server,
    pilots: sampleSeries(
      server.number,
      SAMPLE_CHART_LENGTH,
      SAMPLE_CHART_MAXIMUM,
    ),
    span: SAMPLE_CHART_SPAN_SECONDS,
  };
}

/** Whether the master knows a server that has held no status: created, never registered. */
async function neverRegistered(serverId: number): Promise<boolean> {
  const statuses = await api.servers.getStatuses(serverId);

  return !statuses.error && statuses.data.length === 0;
}

/**
 * Reads one server: its line of the list, and its newest samples, asked at
 * the same time as the list. Nothing when no server has this id. A server
 * just created, which the list does not hold until it registers, is a sheet
 * with no server.
 */
export const getServerSheet = cache(
  async (id: string): Promise<ServerSheet | null> => {
    const serverId = Number(id);
    const [{ source, servers }, samples] = await Promise.all([
      getServerList(),
      Number.isInteger(serverId)
        ? api.servers.getSamples(serverId, MAXIMUM_PAGE_SIZE)
        : null,
    ]);
    const known = servers.find(server => server.id === id) ?? null;

    if (source === 'sample') {
      return known ? sampleSheet(known) : null;
    }
    if (samples === null) {
      return null;
    }
    if (!known) {
      return (await neverRegistered(serverId))
        ? { source, server: null, pilots: [], span: 0 }
        : null;
    }

    const kept = samples.error ? [] : samples.data;
    return {
      source,
      server: known,
      pilots: pilotsOfSamples(kept),
      span: spanOf(kept),
    };
  },
);
