import { describe, expect, it } from 'vitest';
import type { MapServer } from '@/lib/rtype/servers';
import { cycleServer, quickPlayTarget, serversOnMap, starLook } from './rules';

function server(overrides: Partial<MapServer>): MapServer {
  return {
    id: 'nebula',
    name: 'Nebula Lyon',
    status: 'online',
    version: 'v0.1.0',
    compatible: true,
    number: 3,
    ping: 18,
    capacity: 32,
    games: [],
    ...overrides,
  };
}

describe('starLook', () => {
  it('draws a server of another version hollow and dim, whatever its pilots', () => {
    const look = starLook(
      server({ compatible: false, games: [{ pilots: 4, state: 'playing' }] }),
    );

    expect(look).toEqual({ mark: 'off', tone: 'dim' });
  });

  it('draws an empty server as a dim dot', () => {
    expect(starLook(server({}))).toEqual({ mark: 'empty', tone: 'dim' });
  });

  it('draws a cross up to ten pilots and a crowd from eleven', () => {
    const ten = server({
      games: [
        { pilots: 4, state: 'playing' },
        { pilots: 4, state: 'playing' },
        { pilots: 2, state: 'waiting' },
      ],
    });
    const eleven = server({
      games: [...ten.games, { pilots: 1, state: 'waiting' }],
    });

    expect(starLook(ten).mark).toBe('cross');
    expect(starLook(eleven).mark).toBe('crowd');
  });

  it('draws a slow server with pilots in the warning color', () => {
    const slow = server({
      status: 'degraded',
      games: [{ pilots: 2, state: 'waiting' }],
    });

    expect(starLook(slow).tone).toBe('slow');
  });
});

describe('serversOnMap and cycleServer', () => {
  const near = server({ id: 'near', ping: 18 });
  const middle = server({ id: 'middle', ping: 46 });
  const far = server({ id: 'far', ping: 95 });
  const gone = server({ id: 'gone', ping: 5, status: 'offline' });
  const onMap = serversOnMap([far, gone, near, middle]);

  it('leaves the offline servers out and puts the nearest first', () => {
    expect(onMap.map(each => each.id)).toEqual(['near', 'middle', 'far']);
  });

  it('starts from the nearest when none is chosen, and wraps past either end', () => {
    expect(cycleServer(onMap, null, 1)?.id).toBe('near');
    expect(cycleServer(onMap, 'far', 1)?.id).toBe('near');
    expect(cycleServer(onMap, 'near', -1)?.id).toBe('far');
  });

  it('has nothing to choose on an empty map', () => {
    expect(cycleServer([], null, 1)).toBeUndefined();
  });
});

describe('quickPlayTarget', () => {
  it('picks the nearest server with a seat, and its first game with one', () => {
    const fullNear = server({
      id: 'near',
      ping: 18,
      games: [{ pilots: 4, state: 'waiting' }],
    });
    const open = server({
      id: 'open',
      ping: 46,
      games: [
        { pilots: 2, state: 'playing' },
        { pilots: 1, state: 'waiting' },
      ],
    });

    const target = quickPlayTarget([open, fullNear]);

    expect(target?.server.id).toBe('open');
    expect(target?.gameIndex).toBe(1);
  });
});
