import { Heading } from '@/components/primitives/text/typography';
import { SamplesChart } from '@/components/servers/samples-chart/samples-chart';
import { format } from '@/i18n/format';
import { formatDuration } from '@/i18n/format-duration';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The value at the top of the chart when no server is busier than this. */
const MINIMUM_CHART_MAXIMUM = 4;

/** The pilots of a server over time, from its samples; of the examples, over a day. */
export function ServerChart({
  lang,
  name,
  pilots,
  span,
  example,
}: {
  lang: Locale;
  name: string;
  /** The pilots at each sample, oldest first. */
  pilots: readonly number[];
  /** Seconds between the oldest sample and the newest. */
  span: number;
  /** Whether the samples are the examples of the site. */
  example: boolean;
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.serverPage;
  const since = format(copy.since, {
    duration: formatDuration(span, {
      seconds: copy.durationSeconds,
      minutes: copy.durationMinutes,
      hours: copy.durationHours,
    }),
  });

  return (
    <section className="grid gap-4">
      <Heading size="section">
        {example ? copy.chartTitleSample : copy.chartTitle}
      </Heading>
      <SamplesChart
        samples={pilots}
        maximum={Math.max(MINIMUM_CHART_MAXIMUM, ...pilots)}
        startLabel={since}
        endLabel={dictionary.chart.now}
        aria-label={format(copy.chartAria, { name, since })}
      />
    </section>
  );
}
