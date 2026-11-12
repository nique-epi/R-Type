import { format } from '@/i18n/format';
import { formatDuration } from '@/i18n/format-duration';
import type { PageCopy } from '@/i18n/page-copy';

/** When the oldest sample of a server was taken, `span` seconds before the newest: "il y a 10 min". */
export function sinceLabelOf(
  copy: PageCopy['serverPage'],
  span: number,
): string {
  return format(copy.since, {
    duration: formatDuration(span, {
      seconds: copy.durationSeconds,
      minutes: copy.durationMinutes,
      hours: copy.durationHours,
    }),
  });
}
