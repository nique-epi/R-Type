/** A pilot of an example leaderboard; the date is a day and the index of a month of `MONTHS`. */
export type SampleScore = {
  rank: number;
  name: string;
  score: number;
  day: number;
  month: number;
};

/** The months an example score may carry: October, November, December 2026. */
export const MONTHS = 3;

const PILOT_NAMES = [
  'NOVA',
  'CAPTAIN_STARDUST',
  'ZED',
  'KIRA',
  'R9_ARROW',
  'MAVERICK',
  'FORCE_POD',
  'ORION',
  'LYRA',
  'VEGA',
  'WAVE_CANNON',
  'BLUE_LANCE',
  'TITAN',
  'MIRA',
  'ATLAS',
  'GHOST',
  'PIXEL',
  'COMET',
  'RIGEL',
  'SOL',
];

/** The ranks a leaderboard shows. */
export const BOARD_SIZE = 10;

const TOP_SCORE = 248300;
const SCORE_STEP = 1000;
const SCORE_STEPS = 40;
const FIRST_GAP = 3000;
const GAP_STEPS = 18;
const DAYS_IN_A_MONTH = 28;

/** A generator of numbers in [0, 1) that gives the same run for the same seed. */
function randomFrom(seed: number): () => number {
  let state = seed >>> 0;

  return () => {
    state = (Math.imul(state, 1664525) + 1013904223) >>> 0;
    return state / 4294967296;
  };
}

/** The pilots in a shuffled order, the same for the same random run. */
function shuffled(random: () => number): string[] {
  const names = [...PILOT_NAMES];

  for (let index = names.length - 1; index > 0; index -= 1) {
    const other = Math.floor(random() * (index + 1));
    const kept = names[index] ?? '';
    names[index] = names[other] ?? '';
    names[other] = kept;
  }

  return names;
}

/**
 * Example scores, since the master keeps none yet: ten pilots of a made-up
 * leaderboard, best first, the same for the same `seed` and `difficulty`
 * (0 easy, 1 normal, 2 hard).
 */
export function sampleScores(seed: number, difficulty: number): SampleScore[] {
  const random = randomFrom(seed);
  const names = shuffled(random);
  let score =
    TOP_SCORE +
    Math.floor(random() * SCORE_STEPS) * SCORE_STEP * (difficulty + 1);
  const rows: SampleScore[] = [];

  for (let index = 0; index < BOARD_SIZE; index += 1) {
    rows.push({
      rank: index + 1,
      name: names[index] ?? '',
      score,
      day: 1 + Math.floor(random() * DAYS_IN_A_MONTH),
      month: Math.floor(random() * MONTHS),
    });
    score -= FIRST_GAP + Math.floor(random() * GAP_STEPS) * SCORE_STEP;
  }

  return rows;
}
