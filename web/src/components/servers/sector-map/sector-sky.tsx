import { format } from '@/i18n/format';
import {
  MAP_PIXELS,
  PING_RINGS,
  PLANET_RADIUS,
  ringBandPath,
  ringFrontPath,
  ringLabelLeft,
  ringPoints,
  toMapPixels,
  type View,
} from './geometry';

/** The ring of the planet leans 30° on the map, as in the client. */
const RING_TILT = 30;

export type SectorSkyProps = {
  view: View;
  /** "{ping} MS": the label of a ring. */
  ringLabel: string;
};

/**
 * The planet of the reader, dithered and lit from the top right, with its
 * ring; the dashed rings of ping around it, and their labels on the horizon.
 */
export function SectorSky({ view, ringLabel }: SectorSkyProps) {
  const center = toMapPixels({ x: view.cx, y: view.cy });
  const radius = PLANET_RADIUS * view.zoom * 2;

  return (
    <>
      <svg
        data-sector-sky
        viewBox={`0 0 ${MAP_PIXELS.width} ${MAP_PIXELS.height}`}
        aria-hidden="true"
        className="absolute inset-0 size-full overflow-hidden"
      >
        <defs>
          <pattern
            id="sector-deep"
            width="6"
            height="6"
            patternUnits="userSpaceOnUse"
          >
            <rect width="2" height="2" fill="var(--color-deep-blue)" />
            <rect
              x="3"
              y="3"
              width="2"
              height="2"
              fill="var(--color-deep-blue)"
            />
          </pattern>
          <pattern
            id="sector-blue"
            width="6"
            height="6"
            patternUnits="userSpaceOnUse"
          >
            <rect width="2" height="2" fill="var(--color-blue)" />
          </pattern>
          <pattern
            id="sector-pale"
            width="6"
            height="6"
            patternUnits="userSpaceOnUse"
          >
            <rect width="4" height="4" fill="var(--color-pale-blue)" />
            <rect
              x="3"
              y="3"
              width="2"
              height="2"
              fill="var(--color-star-white)"
            />
          </pattern>
          <radialGradient id="sector-sun" cx="0.72" cy="0.18" r="0.85">
            <stop offset="0" stopColor="#ffffff" />
            <stop offset="0.55" stopColor="#ffffff" stopOpacity="0.5" />
            <stop offset="1" stopColor="#ffffff" stopOpacity="0" />
          </radialGradient>
          <mask id="sector-lit">
            <circle r={radius} fill="url(#sector-sun)" />
          </mask>
        </defs>
        <g transform={`translate(${center.x} ${center.y})`}>
          <g transform={`rotate(${RING_TILT})`}>
            <path
              d={ringBandPath(radius)}
              fill="url(#sector-blue)"
              fillRule="evenodd"
            />
          </g>
          <circle r={radius} fill="var(--color-ink)" />
          <circle r={radius} fill="url(#sector-deep)" />
          <circle r={radius} fill="url(#sector-pale)" mask="url(#sector-lit)" />
          <g transform={`rotate(${RING_TILT})`}>
            <path d={ringFrontPath(radius)} fill="url(#sector-pale)" />
          </g>
        </g>
        {PING_RINGS.map(ping => (
          <polyline
            key={ping}
            data-ping-ring
            points={ringPoints(ping, view)}
            fill="none"
            stroke="var(--color-deep-blue)"
            strokeWidth="2"
            strokeDasharray="4 6"
            shapeRendering="crispEdges"
          />
        ))}
      </svg>
      {PING_RINGS.map(ping => {
        const left = ringLabelLeft(ping, view);
        return left === null ? null : (
          <span
            key={ping}
            data-ring-label
            aria-hidden="true"
            className="absolute top-78 small-caps text-blue"
            style={{ left }}
          >
            {format(ringLabel, { ping })}
          </span>
        );
      })}
    </>
  );
}
