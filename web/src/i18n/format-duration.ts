import { format } from './format';

/** The words of a length of time: each a template with a `{count}` hole. */
export type DurationWords = {
  seconds: string;
  minutes: string;
  hours: string;
};

const SECONDS_PER_MINUTE = 60;
const SECONDS_PER_HOUR = 3600;

/** Up to this many seconds a length is given in seconds, then in minutes. */
const SECONDS_LIMIT = 90;
/** Up to this many seconds a length is given in minutes, then in hours. */
const MINUTES_LIMIT = 5400;

/** A length of time in its nearest unit: "45 s", "8 min", "24 h". */
export function formatDuration(seconds: number, words: DurationWords): string {
  if (seconds < SECONDS_LIMIT) {
    return format(words.seconds, { count: Math.round(seconds) });
  }
  if (seconds < MINUTES_LIMIT) {
    return format(words.minutes, {
      count: Math.round(seconds / SECONDS_PER_MINUTE),
    });
  }
  return format(words.hours, {
    count: Math.round(seconds / SECONDS_PER_HOUR),
  });
}
