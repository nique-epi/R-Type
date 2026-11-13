import type * as React from 'react';
import { FULL_PERCENT, type LoadLevel } from '@/lib/rtype/server-load';
import { cn } from '@/lib/utils';

const SEGMENT_COUNT = 10;
const SEGMENTS = Array.from({ length: SEGMENT_COUNT }, (_, index) => index);

const LEVEL_LOOKS: Record<LoadLevel, { word: string; segment: string }> = {
  calm: { word: 'text-player-light-green', segment: 'bg-player-green' },
  busy: { word: 'text-star-gold', segment: 'bg-star-gold' },
  flatOut: { word: 'text-player-light-pink', segment: 'bg-player-pink' },
};

/** The segments of the gauge lit at `percent`: one per ten percent, to the nearest. */
export function litSegmentsOf(percent: number): number {
  const bounded = Math.min(FULL_PERCENT, Math.max(0, percent));

  return Math.round((bounded / FULL_PERCENT) * SEGMENT_COUNT);
}

export type LoadGaugeProps = {
  /** What is measured: "Processeur". */
  label: string;
  /** The measure and its words; nothing when the server did not measure it. */
  reading: {
    /** From 0 to 100. */
    percent: number;
    /** The percent as the page writes it: "94 %". */
    text: string;
    level: LoadLevel;
    /** The level in words, so it is never read from the color alone: "à fond". */
    levelWord: string;
    /** What the percent is a share of: "3,3 / 8 Go". */
    detail?: string;
  } | null;
  /** What stands for the measure of a server that took none: "non mesuré". */
  notMeasured: string;
  /** The curve of the measure over time, under the gauge. */
  children?: React.ReactNode;
};

/**
 * One load of a machine: its percent in pixel digits, a bar of ten segments
 * and its level in words; a screen reader gets the same from a hidden meter. A server that did not measure it shows a dash and
 * says so, never 0 %.
 */
export function LoadGauge({
  label,
  reading,
  notMeasured,
  children,
}: LoadGaugeProps) {
  const look = reading ? LEVEL_LOOKS[reading.level] : null;
  const lit = reading ? litSegmentsOf(reading.percent) : 0;
  const litSegment = look?.segment ?? 'bg-deep-blue';

  return (
    <div
      data-slot="load-gauge"
      data-level={reading?.level ?? 'unmeasured'}
      className="grid content-start gap-3"
    >
      <span
        className={cn(
          'font-pixel text-[32px] leading-8',
          reading ? 'text-star-white' : 'text-pale-blue',
        )}
      >
        {reading ? reading.text : '—'}
      </span>
      {reading ? (
        <meter
          className="sr-only"
          aria-label={label}
          min={0}
          max={FULL_PERCENT}
          value={reading.percent}
        >
          {`${reading.text}, ${reading.levelWord}`}
        </meter>
      ) : null}
      <div aria-hidden="true" className="grid grid-cols-10 gap-1">
        {SEGMENTS.map(segment => (
          <span
            key={segment}
            className={cn('h-3.5', segment < lit ? litSegment : 'bg-deep-blue')}
          />
        ))}
      </div>
      <p className="m-0 small-caps text-pale-blue">
        {label}
        {reading?.detail ? ` · ${reading.detail}` : null}
        {' · '}
        {reading ? (
          <span className={look?.word}>{reading.levelWord}</span>
        ) : (
          notMeasured
        )}
      </p>
      {children}
    </div>
  );
}
