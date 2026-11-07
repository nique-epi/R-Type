'use client';

import { useRef } from 'react';
import { useMotion } from '@/hooks/use-motion';
import { gsap, ScrollTrigger } from '@/lib/gsap';
import { areaPoints, CHART_BOX, stepPoints } from './chart-points';

/** The three rules across the chart: its top, its middle and its floor. */
const GRID_ROWS = [
  CHART_BOX.top,
  (CHART_BOX.top + CHART_BOX.bottom) / 2,
  CHART_BOX.bottom,
];
const AXIS_TEXT = 'fill-pale-blue font-small text-base';

export type SamplesChartProps = {
  /** The samples, oldest first. */
  samples: readonly number[];
  /** The value at the top of the chart. */
  maximum: number;
  /** When the first sample was taken: "il y a 24 h". */
  startLabel: string;
  /** "maintenant". */
  endLabel: string;
  /** What the chart shows, in a sentence, for a screen reader. */
  'aria-label': string;
};

/**
 * The samples of a server in a stepped line on the sunk ground: the line
 * draws itself when the chart scrolls into view, then the area under it
 * fills in, and both draw again when the period changes.
 */
export function SamplesChart({
  samples,
  maximum,
  startLabel,
  endLabel,
  'aria-label': label,
}: SamplesChartProps) {
  const chart = useRef<SVGSVGElement>(null);
  const line = stepPoints(samples, maximum);

  useMotion(
    () => {
      const path =
        chart.current?.querySelector<SVGPolylineElement>('[data-line]');
      if (!path) {
        return;
      }
      const length = path.getTotalLength();
      const draw = () => {
        gsap.fromTo(
          path,
          { strokeDasharray: length, strokeDashoffset: length },
          {
            strokeDashoffset: 0,
            duration: 1.4,
            ease: 'power2.inOut',
            clearProps: 'strokeDasharray,strokeDashoffset',
          },
        );
        gsap.from('[data-area]', {
          opacity: 0,
          duration: 1,
          delay: 0.6,
          ease: 'power2.out',
        });
      };
      ScrollTrigger.create({
        trigger: chart.current,
        start: 'top 85%',
        once: true,
        onEnter: draw,
      });
    },
    { scope: chart, dependencies: [line], revertOnUpdate: true },
  );

  return (
    <svg
      ref={chart}
      role="img"
      aria-label={label}
      viewBox={`0 0 ${CHART_BOX.width} ${CHART_BOX.height}`}
      data-slot="samples-chart"
      className="block h-auto w-full rounded-lg bg-ink"
    >
      {GRID_ROWS.map(y => (
        <rect
          key={y}
          x={CHART_BOX.left}
          y={y}
          width={CHART_BOX.right - CHART_BOX.left}
          height="2"
          className="fill-deep-blue"
        />
      ))}
      <polygon
        data-area
        points={areaPoints(line)}
        className="fill-player-dark-blue opacity-60"
      />
      <polyline
        data-line
        points={line}
        fill="none"
        strokeWidth="2"
        shapeRendering="crispEdges"
        className="stroke-hud-cyan"
      />
      <text x="4" y="28" className={AXIS_TEXT}>
        {maximum}
      </text>
      <text x="4" y="214" className={AXIS_TEXT}>
        0
      </text>
      <text x={CHART_BOX.left} y="234" className={AXIS_TEXT}>
        {startLabel}
      </text>
      <text
        x={CHART_BOX.width - 8}
        y="234"
        textAnchor="end"
        className={AXIS_TEXT}
      >
        {endLabel}
      </text>
    </svg>
  );
}
