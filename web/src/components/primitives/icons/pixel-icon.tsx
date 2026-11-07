import type * as React from 'react';

/** The icons of the site, drawn on a pixel grid so they stay crisp. */
const ICONS = {
  close: {
    grid: 12,
    path: 'M1 1h2v1h1v1h1v1h2V3h1V2h1V1h2v2h-1v1h-1v1H9v2h1v1h1v1h1v2h-2v-1H9v-1H8V9H7V8H5v1H4v1H3v1H1V9h1V8h1V7h1V5H3V4H2V3H1z',
  },
  menu: { grid: 14, path: 'M1 3h12v2H1zM1 6h12v2H1zM1 9h12v2H1z' },
  download: {
    grid: 14,
    path: 'M6 1h2v7h2v1H9v1H8v1H6v-1H5V9H4V8h2z M2 12h10v1H2z',
  },
} as const;

export type PixelIconName = keyof typeof ICONS;

/** A pixel icon in the current text color; it is decoration, the control names itself. */
export function PixelIcon({
  name,
  size = 24,
  ...props
}: Omit<React.ComponentProps<'svg'>, 'viewBox'> & {
  name: PixelIconName;
  size?: number;
}) {
  const icon = ICONS[name];
  return (
    <svg
      width={size}
      height={size}
      viewBox={`0 0 ${icon.grid} ${icon.grid}`}
      aria-hidden="true"
      shapeRendering="crispEdges"
      {...props}
    >
      <path d={icon.path} fill="currentColor" />
    </svg>
  );
}
