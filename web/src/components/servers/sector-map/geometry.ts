/**
 * The geometry of the sector map, as SectorLayout.cpp and
 * SectorMapConstants.hpp of the client lay it out on the 480 × 270 grid of
 * the game screen: the planet is the reader, each server a star around it,
 * further away the higher its ping. Points are in pixels of that grid, and
 * `toMapPixels` turns them into pixels of the map box of the site (×2).
 */

export type Point = { x: number; y: number };

/** Where a server lies from the planet: its bearing in degrees and its ping in ms. */
export type Bearing = { angle: number; ping: number };

/** The part of the sky the map shows: a zoom and the place of the planet. */
export type View = { zoom: number; cx: number; cy: number };

export const MAP_ORIGIN: Point = { x: 24, y: 52 };
export const MAP_SIZE = { width: 432, height: 168 } as const;
export const PLANET_CENTER: Point = { x: 24, y: 136 };
export const PLANET_RADIUS = 60;

/** The pings the dashed rings mark, in milliseconds. */
export const PING_RINGS = [25, 75, 150, 225] as const;

/** The zoom on a chosen server, and where the server then lands on the grid. */
export const CLOSE_ZOOM = 2;
export const CLOSE_TARGET: Point = { x: 300, y: 126 };

/** The whole sky, the planet at its place. */
export const WHOLE_VIEW: View = {
  zoom: 1,
  cx: PLANET_CENTER.x,
  cy: PLANET_CENTER.y,
};

/** The ratio of the short to the long axis of the ring, tilted by 30°. */
export const RING_OPENING =
  0.55 / Math.sqrt((0.5 * 0.835) ** 2 + (0.866 * 0.835) ** 2 + 0.55 * 0.55);

const RING_COS = 0.866;
const RING_SIN = 0.5;
/** The gap between the ring and the closest star, in pixels. */
const RING_MARGIN = 14;
/** The half height of the map the stars spread over, in pixels. */
const SPREAD_HEIGHT = 78;
/** The widest bearing a star takes, in degrees. */
const MAXIMUM_BEARING = 75;
const SPREAD_PER_ROOT_MS = 24.5;
const SPREAD_START_ROOT_MS = 3;
const PING_TO_DISTANCE = 1.75;
const SPREAD_ITERATIONS = 2;
const GOLDEN_HASH = BigInt('0x9E3779B97F4A7C15');
const HASH_SHIFT = BigInt(40);
const HASH_RANGE = 16_777_216;
const RADIANS_PER_DEGREE = Math.PI / 180;

/** How far from the center of the planet the ring ends in that direction, plus the margin. */
export function reachOf(direction: number): number {
  const longHalf = 2 * PLANET_RADIUS;
  const shortHalf = longHalf * RING_OPENING;
  const ax = Math.cos(direction);
  const ay = Math.sin(direction);
  const along = ax * RING_COS + ay * RING_SIN;
  const across = -ax * RING_SIN + ay * RING_COS;
  const ring =
    1 / Math.sqrt((along / longHalf) ** 2 + (across / shortHalf) ** 2);
  return Math.max(PLANET_RADIUS, ring) + RING_MARGIN;
}

/** How far past the ring a ping puts a star: 0 up to 9 ms, then growing as its root. */
export function spreadOf(ping: number): number {
  return Math.max(
    0,
    SPREAD_PER_ROOT_MS * (Math.sqrt(ping) - SPREAD_START_ROOT_MS),
  );
}

/** The direction a star is drawn in, squeezed so far stars stay inside the map. */
export function spreadDirection(angle: number, ping: number): number {
  let direction = angle * RADIANS_PER_DEGREE;
  for (let pass = 0; pass < SPREAD_ITERATIONS; pass++) {
    const distance = reachOf(direction) + spreadOf(ping);
    direction =
      (angle / MAXIMUM_BEARING) *
      Math.asin(Math.min(1, SPREAD_HEIGHT / distance));
  }
  return direction;
}

/**
 * The bearing of a server, drawn from its number by a golden-ratio hash so
 * it never moves between two visits, within the span its ping allows.
 */
export function angleOf(serverNumber: number, ping: number): number {
  const span = Math.min(
    MAXIMUM_BEARING,
    Math.asin(Math.min(1, SPREAD_HEIGHT / (ping * PING_TO_DISTANCE))) /
      RADIANS_PER_DEGREE,
  );
  const hashed = BigInt.asUintN(64, BigInt(serverNumber) * GOLDEN_HASH);
  const share = Number(hashed >> HASH_SHIFT) / HASH_RANGE;
  return Math.round(-span + share * 2 * span);
}

/** Where a star lies on the grid in a view. */
export function placeOf(bearing: Bearing, view: View): Point {
  const direction = spreadDirection(bearing.angle, bearing.ping);
  const distance = reachOf(direction) + spreadOf(bearing.ping);
  return {
    x: Math.round(view.cx + distance * Math.cos(direction) * view.zoom),
    y: Math.round(view.cy + distance * Math.sin(direction) * view.zoom),
  };
}

/** The view that zooms on a star and brings it to the target point. */
export function viewOn(bearing: Bearing, zoom: number, target: Point): View {
  const place = placeOf(bearing, { zoom, cx: 0, cy: 0 });
  return { zoom, cx: target.x - place.x, cy: target.y - place.y };
}

/** A point of the grid in pixels of the map box of the site, which draws the grid at ×2. */
export function toMapPixels(point: Point): Point {
  return { x: (point.x - MAP_ORIGIN.x) * 2, y: (point.y - MAP_ORIGIN.y) * 2 };
}

/** The dashed line of a ping, from the top of the map to its bottom, in map pixels. */
export function ringPoints(ping: number, view: View): string {
  const points: string[] = [];
  for (let degrees = -90; degrees <= 90; degrees += 3) {
    const direction = degrees * RADIANS_PER_DEGREE;
    const distance = (reachOf(direction) + spreadOf(ping)) * view.zoom;
    const point = toMapPixels({
      x: view.cx + distance * Math.cos(direction),
      y: view.cy + distance * Math.sin(direction),
    });
    points.push(`${point.x},${point.y}`);
  }
  return points.join(' ');
}

/** Where a ring crosses the horizon, in map pixels, or null when its label would leave the map. */
export function ringLabelLeft(ping: number, view: View): number | null {
  const crossing =
    view.cx + (reachOf(0) + spreadOf(ping)) * view.zoom - MAP_ORIGIN.x;
  const labelWidth = 34;
  if (crossing <= 4 || crossing >= MAP_SIZE.width - labelWidth) {
    return null;
  }
  return Math.round((crossing + 3) * 2);
}

/** The band of the ring behind the planet, as an even-odd SVG path. */
export function ringBandPath(radius: number): string {
  const outer = 2 * radius;
  const inner = 1.4 * radius;
  const outerShort = outer * RING_OPENING;
  const innerShort = inner * RING_OPENING;
  return (
    `M ${-outer} 0 A ${outer} ${outerShort} 0 1 0 ${outer} 0 A ${outer} ${outerShort} 0 1 0 ${-outer} 0 Z ` +
    `M ${-inner} 0 A ${inner} ${innerShort} 0 1 0 ${inner} 0 A ${inner} ${innerShort} 0 1 0 ${-inner} 0 Z`
  );
}

/** The front half of the ring, drawn over the planet. */
export function ringFrontPath(radius: number): string {
  const outer = 2 * radius;
  const inner = 1.4 * radius;
  return (
    `M ${-outer} 0 A ${outer} ${outer * RING_OPENING} 0 0 0 ${outer} 0 ` +
    `L ${inner} 0 A ${inner} ${inner * RING_OPENING} 0 0 1 ${-inner} 0 Z`
  );
}

/** The mini-map draws the whole sky at a sixth of the grid, ×2 for the site. */
const MINIMAP_DIVISOR = 6;
const MINIMAP_OFFSET = 2;

/** A point of the grid, seen on the whole sky, in pixels of the mini-map. */
export function toMinimapPixels(point: Point): Point {
  return {
    x:
      Math.round(((point.x - MAP_ORIGIN.x) / MINIMAP_DIVISOR) * 2) -
      MINIMAP_OFFSET,
    y:
      Math.round(((point.y - MAP_ORIGIN.y) / MINIMAP_DIVISOR) * 2) -
      MINIMAP_OFFSET,
  };
}

/** The box of the mini-map that frames what a zoomed view shows, in pixels of the mini-map. */
export function minimapFrame(view: View): {
  left: number;
  top: number;
  width: number;
  height: number;
} {
  const shrink = WHOLE_VIEW.zoom / view.zoom;
  const onWhole = (point: Point): Point => ({
    x: WHOLE_VIEW.cx + (point.x - view.cx) * shrink,
    y: WHOLE_VIEW.cy + (point.y - view.cy) * shrink,
  });
  const smallest = 8;
  const start = toMinimapPixels(onWhole(MAP_ORIGIN));
  const end = toMinimapPixels(
    onWhole({
      x: MAP_ORIGIN.x + MAP_SIZE.width,
      y: MAP_ORIGIN.y + MAP_SIZE.height,
    }),
  );
  return {
    left: start.x,
    top: start.y,
    width: Math.max(smallest, end.x - start.x),
    height: Math.max(smallest, end.y - start.y),
  };
}

/** The map box is 864 × 336 pixels; a star closer than 6 pixels to its edge is not drawn. */
export const MAP_PIXELS = { width: 864, height: 336 } as const;
const EDGE_MARGIN = 6;

export function isOnMap(point: Point): boolean {
  return (
    point.x >= EDGE_MARGIN &&
    point.x <= MAP_PIXELS.width - EDGE_MARGIN &&
    point.y >= EDGE_MARGIN &&
    point.y <= MAP_PIXELS.height - EDGE_MARGIN
  );
}
