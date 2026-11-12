import { describe, expect, it } from 'vitest';

import { loadLevelOf, memoryPercentOf, serverLoadOf } from './server-load';

const EIGHT_GIGABYTES = 8_000_000_000;

describe('loadLevelOf', () => {
  it('is calm up to 69 %, loaded from 70 % and flat out from 90 %', () => {
    expect(loadLevelOf(0)).toBe('calm');
    expect(loadLevelOf(69)).toBe('calm');
    expect(loadLevelOf(70)).toBe('busy');
    expect(loadLevelOf(89)).toBe('busy');
    expect(loadLevelOf(90)).toBe('flatOut');
    expect(loadLevelOf(100)).toBe('flatOut');
  });
});

describe('memoryPercentOf', () => {
  it('is the share of the memory in use', () => {
    expect(
      memoryPercentOf({
        measures: {
          usedMemoryBytes: 6_000_000_000,
          totalMemoryBytes: EIGHT_GIGABYTES,
        },
      }),
    ).toBe(75);
  });

  it('is nothing without the memory in use, without all of it, or for a machine of no memory', () => {
    expect(memoryPercentOf({})).toBeUndefined();
    expect(
      memoryPercentOf({ measures: { totalMemoryBytes: EIGHT_GIGABYTES } }),
    ).toBeUndefined();
    expect(
      memoryPercentOf({ measures: { usedMemoryBytes: 1 } }),
    ).toBeUndefined();
    expect(
      memoryPercentOf({
        measures: { usedMemoryBytes: 0, totalMemoryBytes: 0 },
      }),
    ).toBeUndefined();
  });

  it('never passes 100 % when a server says it uses more than it holds', () => {
    expect(
      memoryPercentOf({
        measures: {
          usedMemoryBytes: EIGHT_GIGABYTES + 1,
          totalMemoryBytes: EIGHT_GIGABYTES,
        },
      }),
    ).toBe(100);
  });
});

describe('serverLoadOf', () => {
  it('reads the gauges in the newest sample and the curves oldest first', () => {
    const load = serverLoadOf([
      {
        measures: {
          cpuPercent: 93.6,
          usedMemoryBytes: 2_000_000_000,
          totalMemoryBytes: EIGHT_GIGABYTES,
        },
      },
      {
        measures: {
          cpuPercent: 40,
          usedMemoryBytes: 4_000_000_000,
          totalMemoryBytes: EIGHT_GIGABYTES,
        },
      },
    ]);

    expect(load.cpu).toEqual({ latest: 94, history: [40, 93.6] });
    expect(load.memory).toEqual({ latest: 25, history: [50, 25] });
    expect(load.memoryBytes).toEqual({
      used: 2_000_000_000,
      total: EIGHT_GIGABYTES,
    });
  });

  it('keeps a machine at rest at 0 %, which is a measure', () => {
    expect(serverLoadOf([{ measures: { cpuPercent: 0 } }]).cpu).toEqual({
      latest: 0,
      history: [0],
    });
  });

  it('is not measured for a server whose samples carry no load', () => {
    expect(serverLoadOf([{ measures: {} }, {}])).toEqual({
      cpu: { latest: null, history: [] },
      memory: { latest: null, history: [] },
      memoryBytes: null,
    });
  });

  it('is not measured without a sample', () => {
    expect(serverLoadOf([]).cpu.latest).toBeNull();
    expect(serverLoadOf([]).memoryBytes).toBeNull();
  });

  it('is not measured now when only an older sample measured it, and still draws that one', () => {
    const load = serverLoadOf([{}, { measures: { cpuPercent: 55 } }]);

    expect(load.cpu).toEqual({ latest: null, history: [55] });
  });
});
