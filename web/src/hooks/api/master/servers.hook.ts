import 'server-only';

import { cache } from 'react';
import type { MapServer } from '@/components/servers/sector-map/types';
import { getLatestRelease } from '@/hooks/api/github/latest-release.hook';
import { api, type ListedServer } from '@/lib/api';
import { mapServerOf, type ServerDetails } from '@/lib/rtype/map-server';
import { SAMPLE_SERVERS, sampleSeries } from '@/lib/rtype/sample-servers';

/** Where the servers come from: the master, or the examples while it is silent. */
export type ServerSource = 'master' | 'sample';

export type ServerList = {
  source: ServerSource;
  servers: readonly MapServer[];
};

/**
 * The most servers the pages keep, busiest first: each one costs two reads
 * of the master (its games and its last sample), and the map cannot draw
 * more stars than this and stay readable.
 */
export const MAXIMUM_LISTED_SERVERS = 30;

/** The busiest servers first, those with no pilot last, then by name. */
function busiestFirst(listed: readonly ListedServer[]): ListedServer[] {
  return [...listed].sort(
    (first, second) =>
      second.playerCount - first.playerCount ||
      first.name.localeCompare(second.name),
  );
}

/** The games and the ping of a server; a read that fails leaves it with none. */
async function detailsOf(server: ListedServer): Promise<ServerDetails> {
  const [games, samples] = await Promise.all([
    api.servers.getGames(server.id),
    api.servers.getSamples(server.id, 1),
  ]);

  return {
    games: games.error ? [] : games.data,
    ping: samples.error
      ? undefined
      : samples.data[0]?.measures?.averagePingMilliseconds,
  };
}

/**
 * Reads the servers of the master with their games and their ping. When the
 * master does not answer the list, the examples stand in and `source` says so.
 */
export const getServerList = cache(async (): Promise<ServerList> => {
  const [listing, release] = await Promise.all([
    api.servers.listServers(),
    getLatestRelease(),
  ]);

  if (listing.error) {
    return { source: 'sample', servers: SAMPLE_SERVERS };
  }

  const kept = busiestFirst(listing.data).slice(0, MAXIMUM_LISTED_SERVERS);
  const servers = await Promise.all(
    kept.map(async server =>
      mapServerOf(server, await detailsOf(server), release?.version),
    ),
  );

  return { source: 'master', servers };
});

/** Everything the page of one server shows. */
export type ServerSheet = {
  source: ServerSource;
  /** The server; nothing while it is created and has not registered yet. */
  server: MapServer | null;
  id: string;
  /** The pilots at each sample, oldest first. */
  pilots: readonly number[];
  /** Seconds between the oldest sample and the newest; 0 with fewer than two. */
  span: number;
};

/** The samples the chart draws: one page of the master, the newest. */
const SHEET_SAMPLES = 100;

/** The pilots of an example server's chart: a day, as the mockup draws it. */
const SAMPLE_CHART_LENGTH = 48;
const SAMPLE_CHART_MAXIMUM = 12;
const SAMPLE_CHART_SPAN_SECONDS = 86400;

type SampledSample = { takenAt: number; playerCount: number };

/** The pilots of the samples, which the master lists newest first, oldest first. */
function pilotsOfSamples(samples: readonly SampledSample[]): number[] {
  return samples.map(sample => sample.playerCount).reverse();
}

function spanOf(samples: readonly SampledSample[]): number {
  const newest = samples[0];
  const oldest = samples.at(-1);

  return newest && oldest ? newest.takenAt - oldest.takenAt : 0;
}

/**
 * Reads one server: its line of the list, and its newest samples. Nothing
 * when no server has this id. A server just created, which the list does not
 * hold until it registers, is a sheet with no server.
 */
export async function getServerSheet(id: string): Promise<ServerSheet | null> {
  const { source, servers } = await getServerList();
  const known = servers.find(server => server.id === id) ?? null;

  if (source === 'sample') {
    return known ? sampleSheet(known) : null;
  }

  const serverId = Number(id);

  if (!Number.isInteger(serverId)) {
    return null;
  }

  if (!known) {
    return (await neverRegistered(serverId))
      ? { source, server: null, id, pilots: [], span: 0 }
      : null;
  }

  const samples = await api.servers.getSamples(serverId, SHEET_SAMPLES);
  const kept = samples.error ? [] : samples.data;

  return {
    source,
    server: known,
    id,
    pilots: pilotsOfSamples(kept),
    span: spanOf(kept),
  };
}

function sampleSheet(server: MapServer): ServerSheet {
  return {
    source: 'sample',
    server,
    id: server.id,
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
