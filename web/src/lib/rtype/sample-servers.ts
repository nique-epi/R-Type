import type { MapServer } from '@/components/servers/sector-map/types';

/**
 * The servers shown while the master does not answer: the five of the
 * mockup, one of each kind (busy, free seats, slow, empty, out of service).
 */
export const SAMPLE_SERVERS: readonly MapServer[] = [
  {
    id: 'nebula-lyon',
    name: 'Nebula Lyon',
    status: 'online',
    version: 'v0.1.0',
    compatible: true,
    number: 3,
    ping: 18,
    capacity: 32,
    games: [
      { pilots: 4, state: 'playing' },
      { pilots: 2, state: 'waiting' },
      { pilots: 3, state: 'playing' },
    ],
  },
  {
    id: 'bydo-hunters',
    name: 'Bydo Hunters',
    status: 'online',
    version: 'v0.1.0',
    compatible: true,
    number: 7,
    ping: 46,
    capacity: 32,
    games: [
      { pilots: 4, state: 'playing' },
      { pilots: 3, state: 'waiting' },
    ],
  },
  {
    id: 'epitech-paris-01',
    name: 'Epitech Paris 01',
    status: 'degraded',
    version: 'v0.1.0',
    compatible: true,
    number: 12,
    ping: 95,
    capacity: 32,
    games: [{ pilots: 4, state: 'waiting' }],
  },
  {
    id: 'orbit-night',
    name: 'Orbit Night',
    status: 'online',
    version: 'v0.1.0',
    compatible: true,
    number: 21,
    ping: 150,
    capacity: 32,
    games: [{ pilots: 1, state: 'waiting' }],
  },
  {
    id: 'station-kepler',
    name: 'Station Kepler',
    status: 'offline',
    version: 'v0.0.9',
    compatible: false,
    number: 30,
    ping: 200,
    capacity: 32,
    games: [],
  },
];

const SERIES_SEED_FACTOR = 7919;
const SERIES_MULTIPLIER = 9301;
const SERIES_INCREMENT = 49297;
const SERIES_MODULUS = 233280;
const SERIES_DRIFT = 0.45;
const SERIES_AMPLITUDE = 4;

/**
 * A made-up count of pilots over time, `length` samples from 0 to `maximum`,
 * the same for the same `seed`: the chart of a server of the examples.
 */
export function sampleSeries(
  seed: number,
  length: number,
  maximum: number,
): number[] {
  const values: number[] = [];
  let state = seed * SERIES_SEED_FACTOR;
  let value = Math.round(maximum / 3);

  for (let index = 0; index < length; index += 1) {
    state = (state * SERIES_MULTIPLIER + SERIES_INCREMENT) % SERIES_MODULUS;
    value = Math.max(
      0,
      Math.min(
        maximum,
        value +
          Math.round(
            (state / SERIES_MODULUS - SERIES_DRIFT) * SERIES_AMPLITUDE,
          ),
      ),
    );
    values.push(value);
  }

  return values;
}
