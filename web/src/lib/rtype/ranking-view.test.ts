import { describe, expect, it } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import {
  FIRST_RANKING_CHOICE,
  rankingChoiceAfter,
  rankingFiltersOf,
  rankingRowsOf,
  rankingSummaryOf,
} from './ranking-view';

const COPY = fr.pages.rankingPage;
const SERVERS = ['Nebula Lyon', 'Épitech Paris 01'];
const ON_A_SERVER = { ...FIRST_RANKING_CHOICE, tab: 0 };

describe('rankingRowsOf', () => {
  it('gives ten rows dated in the language of the copy', () => {
    const rows = rankingRowsOf(FIRST_RANKING_CHOICE, COPY);

    expect(rows).toHaveLength(10);
    expect(rows[0]?.date).toMatch(/^\d{2} (OCT|NOV|DEC) 26$/);
  });

  it('gives another board for another difficulty', () => {
    expect(
      rankingRowsOf({ ...FIRST_RANKING_CHOICE, difficulty: 2 }, COPY),
    ).not.toEqual(rankingRowsOf(FIRST_RANKING_CHOICE, COPY));
  });
});

describe('rankingSummaryOf', () => {
  it('names the server, the difficulty and the pilots on the first tab', () => {
    expect(rankingSummaryOf(ON_A_SERVER, SERVERS, COPY, 'fr')).toBe(
      'NEBULA LYON · PARTIES NORMALES, 2 PILOTES',
    );
  });

  it('writes a server without its accent, as the pixel face has none', () => {
    expect(
      rankingSummaryOf({ ...ON_A_SERVER, serverIndex: 1 }, SERVERS, COPY, 'fr'),
    ).toContain('EPITECH PARIS 01');
  });

  it('names no server on the tab of everyone', () => {
    expect(rankingSummaryOf(FIRST_RANKING_CHOICE, SERVERS, COPY, 'fr')).toBe(
      'GLOBAL · PARTIES NORMALES, 2 PILOTES',
    );
  });

  it('says one pilot in the singular', () => {
    expect(
      rankingSummaryOf({ ...FIRST_RANKING_CHOICE, pilots: 1 }, [], COPY, 'fr'),
    ).toBe('GLOBAL · PARTIES NORMALES, 1 PILOTE');
  });

  it('falls back to the tab of everyone when there is no server to name', () => {
    expect(rankingSummaryOf(ON_A_SERVER, [], COPY, 'fr')).toContain('GLOBAL');
  });
});

describe('rankingFiltersOf', () => {
  it('offers the server, the difficulty and the pilots on the first tab', () => {
    expect(
      rankingFiltersOf(ON_A_SERVER, SERVERS, COPY).map(filter => filter.id),
    ).toEqual(['server', 'difficulty', 'pilots']);
  });

  it('offers the difficulty and the pilots on the tab of everyone', () => {
    expect(
      rankingFiltersOf(FIRST_RANKING_CHOICE, SERVERS, COPY).map(
        filter => filter.id,
      ),
    ).toEqual(['difficulty', 'pilots']);
  });

  it('puts each setting at the option in force', () => {
    const filters = rankingFiltersOf(ON_A_SERVER, SERVERS, COPY);

    expect(filters.map(filter => filter.index)).toEqual([0, 1, 1]);
  });
});

describe('rankingChoiceAfter', () => {
  it('reads three settings on the first tab, the pilots as an index from 1', () => {
    expect(rankingChoiceAfter(ON_A_SERVER, SERVERS, [1, 2, 3])).toEqual({
      tab: 0,
      serverIndex: 1,
      difficulty: 2,
      pilots: 4,
    });
  });

  it('reads two settings on the tab of everyone and keeps the server', () => {
    expect(
      rankingChoiceAfter(
        { ...FIRST_RANKING_CHOICE, serverIndex: 1 },
        SERVERS,
        [0, 3],
      ),
    ).toEqual({ tab: 1, serverIndex: 1, difficulty: 0, pilots: 4 });
  });

  it('undoes rankingFiltersOf: the indices it showed give the same choice back', () => {
    const indices = rankingFiltersOf(ON_A_SERVER, SERVERS, COPY).map(
      filter => filter.index,
    );

    expect(rankingChoiceAfter(ON_A_SERVER, SERVERS, indices)).toEqual(
      ON_A_SERVER,
    );
  });
});
