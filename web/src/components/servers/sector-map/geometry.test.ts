import { describe, expect, it } from 'vitest';

import {
  angleOf,
  isOnMap,
  MAP_PIXELS,
  PLANET_RADIUS,
  placeOf,
  reachOf,
  ringLabelLeft,
  ringPoints,
  spreadOf,
  toMapPixels,
  viewOn,
  WHOLE_VIEW,
} from './geometry';

/** The margin of SectorLayout.cpp between the ring and the nearest star. */
const RING_MARGIN = 14;
/** The widest bearing of SectorMapConstants.hpp, in degrees. */
const MAXIMUM_BEARING = 75;

describe('spreadOf', () => {
  it('keeps every server up to 9 ms on the ring', () => {
    expect(spreadOf(0)).toBe(0);
    expect(spreadOf(9)).toBe(0);
  });

  it('grows as the root of the ping past 9 ms: 24.5 pixels per unit of root', () => {
    expect(spreadOf(16)).toBe(24.5);
    expect(spreadOf(25)).toBe(49);
  });
});

describe('reachOf', () => {
  it('never puts a star closer to the planet than its radius and the margin', () => {
    for (let degrees = -90; degrees <= 90; degrees += 5) {
      expect(reachOf((degrees * Math.PI) / 180)).toBeGreaterThanOrEqual(
        PLANET_RADIUS + RING_MARGIN,
      );
    }
  });

  it('is the same in a direction and in the opposite one, as the ring is', () => {
    expect(reachOf(0.4)).toBeCloseTo(reachOf(0.4 + Math.PI), 9);
  });
});

describe('angleOf', () => {
  it('gives a server the same bearing every time', () => {
    expect(angleOf(12, 95)).toBe(angleOf(12, 95));
  });

  it('keeps every bearing within the widest one', () => {
    for (let number = 1; number <= 64; number++) {
      const angle = angleOf(number, 10);
      expect(Math.abs(angle)).toBeLessThanOrEqual(MAXIMUM_BEARING);
    }
  });
});

describe('viewOn', () => {
  it('brings the chosen star to the target point', () => {
    const bearing = { angle: angleOf(7, 46), ping: 46 };
    const target = { x: 300, y: 126 };

    expect(placeOf(bearing, viewOn(bearing, 2, target))).toEqual(target);
  });
});

describe('isOnMap', () => {
  it('draws a star 6 pixels from an edge, and not 5', () => {
    expect(isOnMap({ x: 6, y: 6 })).toBe(true);
    expect(isOnMap({ x: 5, y: 100 })).toBe(false);
    expect(isOnMap({ x: MAP_PIXELS.width - 6, y: MAP_PIXELS.height - 6 })).toBe(
      true,
    );
    expect(isOnMap({ x: MAP_PIXELS.width - 5, y: 100 })).toBe(false);
    expect(isOnMap({ x: 100, y: MAP_PIXELS.height - 5 })).toBe(false);
  });

  it('draws a server of 18 ms on the whole sky', () => {
    const place = toMapPixels(
      placeOf({ angle: angleOf(3, 18), ping: 18 }, WHOLE_VIEW),
    );

    expect(isOnMap(place)).toBe(true);
  });
});

describe('ringLabelLeft', () => {
  it('drops the label of a ring that crosses the horizon off the right of the map', () => {
    expect(ringLabelLeft(225, { ...WHOLE_VIEW, zoom: 2 })).toBeNull();
  });

  it('labels a near ring of the whole sky beyond the edge of the planet', () => {
    const planetEdge = toMapPixels({
      x: WHOLE_VIEW.cx + PLANET_RADIUS,
      y: 0,
    }).x;

    expect(ringLabelLeft(25, WHOLE_VIEW) ?? 0).toBeGreaterThan(planetEdge);
  });
});

describe('ringPoints', () => {
  it('writes every coordinate to hundredths of a pixel at most, so the page hydrates as it was rendered', () => {
    const coordinates = ringPoints(75, WHOLE_VIEW)
      .split(/[ ,]/)
      .map(coordinate => coordinate.split('.')[1]?.length ?? 0);

    expect(Math.max(...coordinates)).toBeLessThanOrEqual(2);
  });

  it('draws one point every three degrees from the top to the bottom of the map', () => {
    expect(ringPoints(25, WHOLE_VIEW).split(' ')).toHaveLength(61);
  });
});
