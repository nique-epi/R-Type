import type { RankingFilter } from '@/components/ranking/ranking-filter-dialog';
import type { RankingRow } from '@/components/ranking/ranking-screen';
import { format, formatCount } from '@/i18n/format';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import { withoutAccents } from './pixel-text';
import { sampleScores } from './sample-scores';

/** The scope and the filters of the leaderboard in force. */
export type RankingChoice = {
  /** 0 for one server, 1 for everyone. */
  tab: number;
  /** Which of the servers the first tab reads. */
  serverIndex: number;
  /** 0 easy, 1 normal, 2 hard. */
  difficulty: number;
  /** From 1 to 4. */
  pilots: number;
};

/** The leaderboard a first visit shows: everyone, normal games, two pilots. */
export const FIRST_RANKING_CHOICE: RankingChoice = {
  tab: 1,
  serverIndex: 0,
  difficulty: 1,
  pilots: 2,
};

const PILOT_OPTIONS = ['1', '2', '3', '4'];
const YEAR = '26';
const SERVER_SEED_STEP = 131;
const PILOTS_SEED_STEP = 17;
const TAB_SEED_STEP = 7919;

type RankingCopy = PageCopy['rankingPage'];

/** The names of the servers as the filter and the summary write them: capitals, no accent. */
function serverNames(servers: readonly string[]): string[] {
  return servers.map(name => withoutAccents(name).toUpperCase());
}

/** Whether the first tab can pick a server: it needs at least one to pick. */
function readsAServer(choice: RankingChoice, servers: readonly string[]) {
  return choice.tab === 0 && servers.length > 0;
}

/** The ten pilots of the example board of this choice, each with their date. */
export function rankingRowsOf(
  choice: RankingChoice,
  copy: RankingCopy,
): RankingRow[] {
  const seed =
    choice.tab * TAB_SEED_STEP +
    choice.serverIndex * SERVER_SEED_STEP +
    choice.pilots * PILOTS_SEED_STEP +
    choice.difficulty;

  return sampleScores(seed, choice.difficulty).map(score => ({
    rank: score.rank,
    name: score.name,
    score: score.score,
    date: `${String(score.day).padStart(2, '0')} ${copy.months[score.month] ?? ''} ${YEAR}`,
  }));
}

/** What the list shows under it: "NEBULA LYON · PARTIES NORMALES, 2 PILOTES". */
export function rankingSummaryOf(
  choice: RankingChoice,
  servers: readonly string[],
  copy: RankingCopy,
  locale: Locale,
): string {
  const values = {
    difficulty: copy.difficultiesPlural[choice.difficulty] ?? '',
    pilots: formatCount(choice.pilots, copy.pilotCount, locale),
  };

  return readsAServer(choice, servers)
    ? format(copy.summaryServer, {
        ...values,
        server: serverNames(servers)[choice.serverIndex] ?? '',
      })
    : format(copy.summaryGlobal, values);
}

/** The settings of the filter dialog: the server on the first tab, then the difficulty and the pilots. */
export function rankingFiltersOf(
  choice: RankingChoice,
  servers: readonly string[],
  copy: RankingCopy,
): RankingFilter[] {
  const common: RankingFilter[] = [
    {
      id: 'difficulty',
      label: copy.filters.difficulty,
      options: copy.difficulties,
      index: choice.difficulty,
    },
    {
      id: 'pilots',
      label: copy.filters.pilots,
      options: PILOT_OPTIONS,
      index: choice.pilots - 1,
    },
  ];

  return readsAServer(choice, servers)
    ? [
        {
          id: 'server',
          label: copy.filters.server,
          options: serverNames(servers),
          index: choice.serverIndex,
        },
        ...common,
      ]
    : common;
}

/** The choice after the filter dialog applied `indices`, one per setting it showed, in its order. */
export function rankingChoiceAfter(
  choice: RankingChoice,
  servers: readonly string[],
  indices: readonly number[],
): RankingChoice {
  const [first = 0, second = 0, third = 0] = indices;

  return readsAServer(choice, servers)
    ? {
        ...choice,
        serverIndex: first,
        difficulty: second,
        pilots: third + 1,
      }
    : { ...choice, difficulty: first, pilots: second + 1 };
}
