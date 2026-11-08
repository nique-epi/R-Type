import type { RankingFilter } from '@/components/ranking/ranking-filter-dialog';
import type { RankingRow } from '@/components/ranking/ranking-screen';
import { format, formatCount } from '@/i18n/format';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import {
  LEADERBOARD_DIFFICULTIES,
  type LeaderboardDifficulty,
  MAXIMUM_LEADERBOARD_PLAYERS,
} from '@/lib/api/schemas/master/leaderboard-names';
import { withoutAccents } from './pixel-text';
import { PLAYER_NUMBERS } from './players';

/** What a leaderboard line holds that the screen draws; the rest of the master's line stays on the server. */
export type RankedPlayer = {
  rank: number;
  nickname: string;
  score: number;
  /** Seconds since 1970 UTC. */
  playedAt: number;
};

/** A server the first tab can read: its id on the master, and its name. */
export type RankingServer = { id: number; name: string };

/** The scope and the filters of the leaderboard in force; they live in the address of the page. */
export type RankingChoice = {
  scope: 'server' | 'global';
  /** The server of the first tab; none on the global tab. */
  serverId: number | null;
  difficulty: LeaderboardDifficulty;
  /** From 1 to 4. */
  pilots: number;
};

/** The leaderboard a first visit shows: everyone, normal games, two pilots. */
export const FIRST_RANKING_CHOICE: RankingChoice = {
  scope: 'global',
  serverId: null,
  difficulty: 'normal',
  pilots: 2,
};

/** The best players the screen lists. */
export const RANKING_LENGTH = 10;

const PILOT_OPTIONS = PLAYER_NUMBERS.map(String);
const TWO_DIGITS = 2;
const YEAR_DIGITS = 100;

type RankingCopy = PageCopy['rankingPage'];

/** The query of an address, as the page receives it. */
export type RankingSearch = Record<string, string | string[] | undefined>;

function firstOf(value: string | string[] | undefined): string | undefined {
  return Array.isArray(value) ? value[0] : value;
}

/** Whether the first tab reads a server of the list. */
function readsAServer(choice: RankingChoice): boolean {
  return choice.scope === 'server' && choice.serverId !== null;
}

/**
 * The choice an address holds. What it gets wrong falls back to the first
 * visit's value, and the first tab, which needs a server, reads the first of
 * the list when the address names none that exists; with no server at all,
 * it is the global tab.
 */
export function rankingChoiceOf(
  search: RankingSearch,
  servers: readonly RankingServer[],
): RankingChoice {
  const difficulty =
    LEADERBOARD_DIFFICULTIES.find(
      name => name === firstOf(search.difficulty),
    ) ?? FIRST_RANKING_CHOICE.difficulty;
  const pilots = Number(firstOf(search.pilots));
  const validPilots =
    Number.isInteger(pilots) &&
    pilots >= 1 &&
    pilots <= MAXIMUM_LEADERBOARD_PLAYERS;
  const requested = Number(firstOf(search.server));
  const server = servers.find(({ id }) => id === requested) ?? servers[0];
  const common = {
    difficulty,
    pilots: validPilots ? pilots : FIRST_RANKING_CHOICE.pilots,
  };

  return firstOf(search.scope) === 'server' && server
    ? { scope: 'server', serverId: server.id, ...common }
    : { scope: 'global', serverId: null, ...common };
}

/** The query that holds a choice, for the address of the page. */
export function rankingQueryOf(choice: RankingChoice): string {
  const query = new URLSearchParams({ scope: choice.scope });

  if (readsAServer(choice)) {
    query.set('server', String(choice.serverId));
  }
  query.set('difficulty', choice.difficulty);
  query.set('pilots', String(choice.pilots));

  return `?${query.toString()}`;
}

/** The tab a choice reads: 0 for a server, 1 for everyone. */
export function rankingTabOf(choice: RankingChoice): number {
  return choice.scope === 'server' ? 0 : 1;
}

/** The choice after a click on a tab. */
export function rankingChoiceOfTab(
  choice: RankingChoice,
  tab: number,
): RankingChoice {
  return { ...choice, scope: tab === 0 ? 'server' : 'global' };
}

function twoDigits(value: number): string {
  return String(value).padStart(TWO_DIGITS, '0');
}

/** "12 NOV 26", the day the score was made, in UTC as the master keeps it. */
function dateOf(seconds: number, copy: RankingCopy): string {
  const day = new Date(seconds * 1000);
  const month = copy.months[day.getUTCMonth()] ?? '';

  return `${twoDigits(day.getUTCDate())} ${month} ${twoDigits(day.getUTCFullYear() % YEAR_DIGITS)}`;
}

/** The lines the screen draws for the players of a leaderboard, in the order the master ranks them. */
export function rankingRowsOf(
  players: readonly RankedPlayer[],
  copy: RankingCopy,
): RankingRow[] {
  return players.map(player => ({
    rank: player.rank,
    name: withoutAccents(player.nickname),
    score: player.score,
    date: dateOf(player.playedAt, copy),
  }));
}

/** The name of a server as the filter and the summary write it: capitals, no accent. */
function serverNameOf(server: RankingServer): string {
  return withoutAccents(server.name).toUpperCase();
}

/** What the list shows under it: "NEBULA LYON · NORMAL GAMES, 2 PILOTS". */
export function rankingSummaryOf(
  choice: RankingChoice,
  servers: readonly RankingServer[],
  copy: RankingCopy,
  locale: Locale,
): string {
  const values = {
    difficulty:
      copy.difficultiesPlural[
        LEADERBOARD_DIFFICULTIES.indexOf(choice.difficulty)
      ] ?? '',
    pilots: formatCount(choice.pilots, copy.pilotCount, locale),
  };
  const server = servers.find(({ id }) => id === choice.serverId);

  return readsAServer(choice) && server
    ? format(copy.summaryServer, { ...values, server: serverNameOf(server) })
    : format(copy.summaryGlobal, values);
}

/** The settings of the filter dialog: the server on the first tab, then the difficulty and the pilots. */
export function rankingFiltersOf(
  choice: RankingChoice,
  servers: readonly RankingServer[],
  copy: RankingCopy,
): RankingFilter[] {
  const common: RankingFilter[] = [
    {
      id: 'difficulty',
      label: copy.filters.difficulty,
      options: copy.difficulties,
      index: LEADERBOARD_DIFFICULTIES.indexOf(choice.difficulty),
    },
    {
      id: 'pilots',
      label: copy.filters.pilots,
      options: PILOT_OPTIONS,
      index: choice.pilots - 1,
    },
  ];

  return readsAServer(choice)
    ? [
        {
          id: 'server',
          label: copy.filters.server,
          options: servers.map(serverNameOf),
          index: Math.max(
            0,
            servers.findIndex(({ id }) => id === choice.serverId),
          ),
        },
        ...common,
      ]
    : common;
}

/** The choice after the filter dialog applied `indices`, one per setting it showed, in its order. */
export function rankingChoiceAfter(
  choice: RankingChoice,
  servers: readonly RankingServer[],
  indices: readonly number[],
): RankingChoice {
  const [first = 0, second = 0, third = 0] = indices;
  const [difficulty, pilots] = readsAServer(choice)
    ? [second, third]
    : [first, second];

  return {
    ...choice,
    serverId: readsAServer(choice)
      ? (servers[first]?.id ?? choice.serverId)
      : choice.serverId,
    difficulty: LEADERBOARD_DIFFICULTIES[difficulty] ?? choice.difficulty,
    pilots: pilots + 1,
  };
}
