import { describe, expect, it } from 'vitest';

import { frPages } from '@/i18n/dictionaries/fr-pages';
import {
  FIRST_RANKING_CHOICE,
  rankingChoiceAfter,
  rankingChoiceOf,
  rankingChoiceOfTab,
  rankingFiltersOf,
  rankingQueryOf,
  rankingRowsOf,
  rankingSummaryOf,
  rankingTabOf,
} from './ranking-view';

const COPY = frPages.rankingPage;
const SERVERS = [
  { id: 7, name: 'Nebula Lyon' },
  { id: 12, name: 'Éclair Paris' },
];
const ON_A_SERVER = {
  ...FIRST_RANKING_CHOICE,
  scope: 'server' as const,
  serverId: 12,
};

describe('rankingChoiceOf', () => {
  it('reads the first visit from an address with no query', () => {
    expect(rankingChoiceOf({}, SERVERS)).toEqual(FIRST_RANKING_CHOICE);
  });

  it('reads the scope, the server, the difficulty and the pilots of the query', () => {
    expect(
      rankingChoiceOf(
        { scope: 'server', server: '12', difficulty: 'hard', pilots: '4' },
        SERVERS,
      ),
    ).toEqual({
      scope: 'server',
      serverId: 12,
      difficulty: 'hard',
      pilots: 4,
    });
  });

  it('falls back to the first visit for a difficulty or a number of pilots it does not know', () => {
    expect(
      rankingChoiceOf({ difficulty: 'nightmare', pilots: '5' }, SERVERS),
    ).toEqual(FIRST_RANKING_CHOICE);
    expect(rankingChoiceOf({ pilots: '0' }, SERVERS).pilots).toBe(2);
    expect(rankingChoiceOf({ pilots: '2.5' }, SERVERS).pilots).toBe(2);
  });

  it('reads the first server of the list when the query names none that exists', () => {
    expect(
      rankingChoiceOf({ scope: 'server', server: '99' }, SERVERS),
    ).toMatchObject({ scope: 'server', serverId: 7 });
  });

  it('reads the global tab when it asks for a server and the master has none', () => {
    expect(rankingChoiceOf({ scope: 'server' }, [])).toEqual(
      FIRST_RANKING_CHOICE,
    );
  });

  it('keeps the first value of a parameter given twice', () => {
    expect(
      rankingChoiceOf({ difficulty: ['easy', 'hard'] }, SERVERS).difficulty,
    ).toBe('easy');
  });
});

describe('rankingQueryOf', () => {
  it('holds a choice so that rankingChoiceOf reads it back', () => {
    expect(
      rankingChoiceOf(
        Object.fromEntries(new URLSearchParams(rankingQueryOf(ON_A_SERVER))),
        SERVERS,
      ),
    ).toEqual(ON_A_SERVER);
  });

  it('names no server on the global tab', () => {
    expect(rankingQueryOf(FIRST_RANKING_CHOICE)).toBe(
      '?scope=global&difficulty=normal&pilots=2',
    );
  });

  it('leaves the server out once the global tab is chosen, though the choice still holds it', () => {
    expect(rankingQueryOf({ ...ON_A_SERVER, scope: 'global' })).toBe(
      '?scope=global&difficulty=normal&pilots=2',
    );
  });
});

describe('rankingTabOf', () => {
  it('is the first tab for a server and the second for everyone', () => {
    expect(rankingTabOf(ON_A_SERVER)).toBe(0);
    expect(rankingTabOf(FIRST_RANKING_CHOICE)).toBe(1);
  });

  it('is undone by rankingChoiceOfTab', () => {
    expect(rankingChoiceOfTab(FIRST_RANKING_CHOICE, 0).scope).toBe('server');
    expect(rankingChoiceOfTab(ON_A_SERVER, 1).scope).toBe('global');
  });
});

describe('rankingRowsOf', () => {
  it('keeps the rank, the score and the order of the master, ties included', () => {
    const rows = rankingRowsOf(
      [
        { rank: 1, nickname: 'ACE', score: 900, playedAt: 0 },
        { rank: 1, nickname: 'R9', score: 900, playedAt: 0 },
        { rank: 3, nickname: 'ZED', score: 100, playedAt: 0 },
      ],
      COPY,
    );

    expect(rows.map(row => [row.rank, row.name, row.score])).toEqual([
      [1, 'ACE', 900],
      [1, 'R9', 900],
      [3, 'ZED', 100],
    ]);
  });

  it('writes the day in UTC with the month of the language and two digits of the year', () => {
    // 2026-11-07 23:59:59 UTC: a clock in a later zone would already say the 8th.
    const [row] = rankingRowsOf(
      [{ rank: 1, nickname: 'ACE', score: 1, playedAt: 1794095999 }],
      COPY,
    );

    expect(row?.date).toBe('07 NOV 26');
  });

  it('draws a nickname without the accents the pixel font lacks', () => {
    const [row] = rankingRowsOf(
      [{ rank: 1, nickname: 'ÉLODIE', score: 1, playedAt: 0 }],
      COPY,
    );

    expect(row?.name).toBe('ELODIE');
  });
});

describe('rankingSummaryOf', () => {
  it('names everyone, the difficulty and the pilots on the global tab', () => {
    expect(rankingSummaryOf(FIRST_RANKING_CHOICE, SERVERS, COPY, 'fr')).toBe(
      'GLOBAL · PARTIES NORMALES, 2 PILOTES',
    );
  });

  it('names the server in capitals without accents on the first tab', () => {
    expect(rankingSummaryOf(ON_A_SERVER, SERVERS, COPY, 'fr')).toBe(
      'ECLAIR PARIS · PARTIES NORMALES, 2 PILOTES',
    );
  });

  it('counts one pilot in the singular', () => {
    expect(
      rankingSummaryOf({ ...FIRST_RANKING_CHOICE, pilots: 1 }, [], COPY, 'fr'),
    ).toContain('1 PILOTE');
  });
});

describe('rankingFiltersOf', () => {
  it('offers the difficulty and the pilots on the global tab', () => {
    expect(
      rankingFiltersOf(FIRST_RANKING_CHOICE, SERVERS, COPY).map(
        filter => filter.id,
      ),
    ).toEqual(['difficulty', 'pilots']);
  });

  it('offers the server first on the first tab, at the index of the one read', () => {
    const [server] = rankingFiltersOf(ON_A_SERVER, SERVERS, COPY);

    expect(server).toMatchObject({ id: 'server', index: 1 });
    expect(server?.options).toEqual(['NEBULA LYON', 'ECLAIR PARIS']);
  });
});

describe('rankingChoiceAfter', () => {
  it('reads the difficulty and the pilots from the two settings of the global tab', () => {
    expect(rankingChoiceAfter(FIRST_RANKING_CHOICE, SERVERS, [2, 3])).toEqual({
      ...FIRST_RANKING_CHOICE,
      difficulty: 'hard',
      pilots: 4,
    });
  });

  it('reads the server too from the three settings of the first tab', () => {
    expect(rankingChoiceAfter(ON_A_SERVER, SERVERS, [0, 0, 0])).toEqual({
      scope: 'server',
      serverId: 7,
      difficulty: 'easy',
      pilots: 1,
    });
  });
});
