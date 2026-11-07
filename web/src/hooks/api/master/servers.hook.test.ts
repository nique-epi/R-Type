import { afterEach, describe, expect, it, vi } from 'vitest';
import { RELEASE_V0_1_0 } from '@/lib/github/release.fixture';
import { answer, listedServer } from '@/test/responses';
import {
  getMappedServerList,
  getServerList,
  getServerNames,
  getServerSheet,
} from './servers.hook';

type Routes = Record<string, Response | Error>;

/** Stubs `fetch` with the master and GitHub: a path that is not listed is a 404. */
function stubNetwork(routes: Routes) {
  vi.stubGlobal(
    'fetch',
    vi.fn(async (input: URL | string) => {
      const url = new URL(String(input));
      const route = routes[`${url.pathname}${url.search}`];

      if (route instanceof Error) {
        throw route;
      }

      return route?.clone() ?? answer(404, { status: 404, title: 'Not Found' });
    }),
  );
}

const GAMES = {
  games: [{ number: 1, name: 'GAME 1', state: 'waiting', playerCount: 2 }],
};
const SAMPLES = {
  samples: [
    {
      takenAt: 100,
      gameCount: 1,
      playerCount: 2,
      measures: { averagePingMilliseconds: 42 },
    },
  ],
};

function masterRoutes(): Routes {
  return {
    '/repos/nique-epi/R-Type/releases/latest': answer(200, RELEASE_V0_1_0),
    '/api/servers?limit=100&offset=0': answer(200, {
      servers: [listedServer(7), { ...listedServer(8), playerCount: 9 }],
    }),
    '/api/servers/7/games': answer(200, GAMES),
    '/api/servers/8/games': answer(200, GAMES),
    '/api/servers/7/samples?limit=1': answer(200, SAMPLES),
    '/api/servers/8/samples?limit=1': answer(200, { samples: [] }),
  };
}

describe('getServerList', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('draws the servers of the master with their games, busiest first', async () => {
    stubNetwork(masterRoutes());

    const list = await getServerList();

    expect(list.source).toBe('master');
    expect(list.servers.map(server => server.id)).toEqual(['8', '7']);
    expect(list.servers[1]?.games).toEqual([{ pilots: 2, state: 'waiting' }]);
  });

  it('asks no sample of a server when the ping is not wanted', async () => {
    stubNetwork(masterRoutes());

    await getServerList();

    const urls = vi.mocked(fetch).mock.calls.map(call => String(call[0]));
    expect(urls.filter(url => url.includes('/samples'))).toEqual([]);
  });

  it('takes the ping of the last sample for the map, and a distance of its own without one', async () => {
    stubNetwork(masterRoutes());

    const { servers } = await getMappedServerList();

    expect(servers.find(server => server.id === '7')?.ping).toBe(42);
    expect(servers.find(server => server.id === '8')?.ping).toBe(75);
  });

  it('keeps a server whose games the master fails to give, without games', async () => {
    stubNetwork({
      ...masterRoutes(),
      '/api/servers/7/games': answer(500, { status: 500, title: 'Error' }),
    });

    const { servers } = await getServerList();

    expect(servers.find(server => server.id === '7')?.games).toEqual([]);
  });

  it('gives the examples, and says so, when the master cannot be reached', async () => {
    stubNetwork({ '/api/servers?limit=100&offset=0': new TypeError('down') });

    const list = await getServerList();

    expect(list.source).toBe('sample');
    expect(list.servers.length).toBeGreaterThan(0);
  });

  it('gives no server, not the examples, when the master answers an empty list', async () => {
    stubNetwork({
      '/api/servers?limit=100&offset=0': answer(200, { servers: [] }),
    });

    expect(await getServerList()).toEqual({ source: 'master', servers: [] });
  });
});

describe('getServerNames', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('gives the names of the servers, busiest first, from the list alone', async () => {
    stubNetwork(masterRoutes());

    expect(await getServerNames()).toEqual(['Server 8', 'Server 7']);
    const urls = vi.mocked(fetch).mock.calls.map(call => String(call[0]));
    expect(urls.every(url => url.includes('/api/servers?'))).toBe(true);
  });

  it('gives the names of the examples when the master cannot be reached', async () => {
    stubNetwork({ '/api/servers?limit=100&offset=0': new TypeError('down') });

    expect(await getServerNames()).toContain('Nebula Lyon');
  });
});

describe('getServerSheet', () => {
  afterEach(() => {
    vi.unstubAllGlobals();
  });

  it('gives the pilots of the samples, oldest first, and the time they span', async () => {
    stubNetwork({
      ...masterRoutes(),
      '/api/servers/7/samples?limit=100': answer(200, {
        samples: [
          { takenAt: 130, gameCount: 1, playerCount: 4 },
          { takenAt: 100, gameCount: 1, playerCount: 2 },
        ],
      }),
    });

    const sheet = await getServerSheet('7');

    expect(sheet?.pilots).toEqual([2, 4]);
    expect(sheet?.span).toBe(30);
  });

  it('is nothing for an id that is no server of the master', async () => {
    stubNetwork(masterRoutes());

    expect(await getServerSheet('99')).toBeNull();
    expect(await getServerSheet('lyon')).toBeNull();
  });

  it('is a sheet with no server for a server created and never registered', async () => {
    stubNetwork({
      ...masterRoutes(),
      '/api/servers/9/statuses': answer(200, { statuses: [] }),
    });

    expect(await getServerSheet('9')).toEqual({
      source: 'master',
      server: null,
      pilots: [],
      span: 0,
    });
  });

  it('is nothing for a server that held a status but is not on the list', async () => {
    stubNetwork({
      ...masterRoutes(),
      '/api/servers/9/statuses': answer(200, {
        statuses: [{ status: 'online', since: 1 }],
      }),
    });

    expect(await getServerSheet('9')).toBeNull();
  });

  it('gives an example server its example chart when the master is silent', async () => {
    stubNetwork({ '/api/servers?limit=100&offset=0': new TypeError('down') });

    const sheet = await getServerSheet('nebula-lyon');

    expect(sheet?.source).toBe('sample');
    expect(sheet?.pilots).toHaveLength(48);
  });
});
