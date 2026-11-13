/** From this share of a machine in use, a server is drawn as loaded. */
export const BUSY_LOAD_PERCENT = 70;
/** From this share of a machine in use, a server is drawn as running flat out. */
export const FLAT_OUT_LOAD_PERCENT = 90;

/** The share of a machine none of which is free. */
export const FULL_PERCENT = 100;

export type LoadLevel = 'calm' | 'busy' | 'flatOut';

/** How loaded a machine is at `percent` of its processors or of its memory. */
export function loadLevelOf(percent: number): LoadLevel {
  if (percent >= FLAT_OUT_LOAD_PERCENT) {
    return 'flatOut';
  }
  return percent >= BUSY_LOAD_PERCENT ? 'busy' : 'calm';
}

/** What a sample says of the machine of its server; a server that does not measure it leaves it out. */
type LoadMeasures = {
  cpuPercent?: number;
  usedMemoryBytes?: number;
  totalMemoryBytes?: number;
};

type LoadSample = { measures?: LoadMeasures };

/** The memory of a machine, in bytes. */
export type MemoryBytes = { used: number; total: number };

/**
 * The memory a sample measured. Nothing when it lacks the memory in use or
 * all of it, or says the machine holds none.
 */
function memoryOf(sample: LoadSample | undefined): MemoryBytes | null {
  const used = sample?.measures?.usedMemoryBytes;
  const total = sample?.measures?.totalMemoryBytes;

  return used === undefined || total === undefined || total <= 0
    ? null
    : { used, total };
}

/** The share of its memory a machine uses, from 0 to 100; nothing when the sample did not measure it. */
export function memoryPercentOf(sample: LoadSample): number | undefined {
  const memory = memoryOf(sample);

  return memory
    ? Math.min(FULL_PERCENT, (memory.used / memory.total) * FULL_PERCENT)
    : undefined;
}

/** One load over the samples of a server. */
export type LoadReading = {
  /** The whole percent of the newest sample; null when it was not measured. */
  latest: number | null;
  /** The percents of the samples that measured it, oldest first. */
  history: readonly number[];
};

/** The load of the machine of a server: its processors and its memory. */
export type ServerLoad = {
  cpu: LoadReading;
  memory: LoadReading;
  /** The memory of the newest sample; null when it was not measured. */
  memoryBytes: MemoryBytes | null;
};

function readingOf(
  samples: readonly LoadSample[],
  percentOf: (sample: LoadSample) => number | undefined,
): LoadReading {
  const newest = samples[0] ? percentOf(samples[0]) : undefined;

  return {
    latest: newest === undefined ? null : Math.round(newest),
    history: samples
      .map(percentOf)
      .filter(percent => percent !== undefined)
      .reverse(),
  };
}

/**
 * The load of a server from its samples, which the master lists newest first.
 * The gauges read the newest sample alone: an older measure is not the load
 * of the server now, so a server that stopped measuring reads "not measured".
 */
export function serverLoadOf(samples: readonly LoadSample[]): ServerLoad {
  return {
    cpu: readingOf(samples, sample => sample.measures?.cpuPercent),
    memory: readingOf(samples, memoryPercentOf),
    memoryBytes: memoryOf(samples[0]),
  };
}
