import { Heading } from '@/components/primitives/text/typography';
import { LoadCurve } from '@/components/servers/load/load-curve';
import {
  LoadGauge,
  type LoadGaugeProps,
} from '@/components/servers/load/load-gauge';
import { format } from '@/i18n/format';
import { formatGigabytes } from '@/i18n/format-size';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import type { PageCopy } from '@/i18n/page-copy';
import {
  FLAT_OUT_LOAD_PERCENT,
  type LoadReading,
  loadLevelOf,
  type ServerLoad as MachineLoad,
} from '@/lib/rtype/server-load';
import { sinceLabelOf } from './since-label';

/** A curve needs two samples: one alone is the gauge above it. */
const FEWEST_CURVE_SAMPLES = 2;

function readingOf(
  copy: PageCopy['serverPage'],
  percent: number | null,
  detail?: string,
): LoadGaugeProps['reading'] {
  if (percent === null) {
    return null;
  }
  const level = loadLevelOf(percent);

  return {
    percent,
    text: format(copy.loadPercent, { percent }),
    level,
    levelWord: copy.loadLevels[level],
    detail,
  };
}

/**
 * The load of the machine of a server: its processors and its memory, each
 * as a gauge of the newest sample over the curve of the samples the page
 * read. A server that sends no such measure reads "not measured".
 */
export function ServerLoad({
  lang,
  name,
  load,
  span,
}: {
  lang: Locale;
  name: string;
  load: MachineLoad;
  /** Seconds between the oldest sample and the newest. */
  span: number;
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.serverPage;
  const since = sinceLabelOf(copy, span);
  const curveOf = (reading: LoadReading, label: string) =>
    reading.history.length >= FEWEST_CURVE_SAMPLES ? (
      <LoadCurve
        percents={reading.history}
        threshold={FLAT_OUT_LOAD_PERCENT}
        startLabel={since}
        endLabel={dictionary.chart.now}
        aria-label={format(copy.loadCurveAria, { label, name, since })}
      />
    ) : null;

  return (
    <section className="grid gap-4">
      <Heading size="section">{copy.loadTitle}</Heading>
      <div className="grid grid-cols-[repeat(auto-fit,minmax(260px,1fr))] gap-8">
        <LoadGauge
          label={copy.cpu}
          reading={readingOf(copy, load.cpu.latest)}
          notMeasured={copy.cpuNotMeasured}
        >
          {curveOf(load.cpu, copy.cpu)}
        </LoadGauge>
        <LoadGauge
          label={copy.memory}
          reading={readingOf(
            copy,
            load.memory.latest,
            load.memoryBytes
              ? format(copy.memoryBytes, {
                  used: formatGigabytes(load.memoryBytes.used, lang),
                  total: formatGigabytes(load.memoryBytes.total, lang),
                })
              : undefined,
          )}
          notMeasured={copy.memoryNotMeasured}
        >
          {curveOf(load.memory, copy.memory)}
        </LoadGauge>
      </div>
    </section>
  );
}
