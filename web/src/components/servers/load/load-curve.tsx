import {
  areaPoints,
  type ChartBox,
  heightOf,
  stepPoints,
} from '@/components/servers/samples-chart/chart-points';
import { FULL_PERCENT } from '@/lib/rtype/server-load';

/** The box of the curve, in units of its view box: 520 × 96. */
export const CURVE_BOX: ChartBox = {
  width: 520,
  height: 96,
  left: 0,
  right: 520,
  top: 8,
  bottom: 88,
};

export type LoadCurveProps = {
  /** The percents of the samples, oldest first. */
  percents: readonly number[];
  /** The percent a machine runs flat out from, drawn as a dashed rule. */
  threshold: number;
  /** When the first sample was taken: "il y a 10 min". */
  startLabel: string;
  /** "maintenant". */
  endLabel: string;
  /** What the curve shows, in a sentence, for a screen reader. */
  'aria-label': string;
};

/** A load over the samples of a server, against 100 %, under its gauge. */
export function LoadCurve({
  percents,
  threshold,
  startLabel,
  endLabel,
  'aria-label': label,
}: LoadCurveProps) {
  const line = stepPoints(percents, FULL_PERCENT, CURVE_BOX);
  const thresholdHeight = heightOf(threshold, FULL_PERCENT, CURVE_BOX);

  return (
    <div data-slot="load-curve" className="grid gap-2">
      <svg
        role="img"
        aria-label={label}
        viewBox={`0 0 ${CURVE_BOX.width} ${CURVE_BOX.height}`}
        className="block h-auto w-full rounded-lg bg-ink"
      >
        <polygon
          points={areaPoints(line, CURVE_BOX)}
          className="fill-player-dark-blue opacity-60"
        />
        <line
          x1={CURVE_BOX.left}
          x2={CURVE_BOX.right}
          y1={thresholdHeight}
          y2={thresholdHeight}
          strokeWidth="2"
          strokeDasharray="6 6"
          className="stroke-player-pink"
        />
        <polyline
          points={line}
          fill="none"
          strokeWidth="2"
          shapeRendering="crispEdges"
          className="stroke-hud-cyan"
        />
      </svg>
      <div className="flex justify-between small-caps text-pale-blue">
        <span>{startLabel}</span>
        <span>{endLabel}</span>
      </div>
    </div>
  );
}
