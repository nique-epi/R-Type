import { CutFrame } from '@/components/game/cut-frame';
import {
  minimapFrame,
  PLANET_CENTER,
  type Point,
  toMinimapPixels,
  type View,
} from './geometry';

const PLANET_DIAMETER = 40;

export type MinimapStar = { id: string; place: Point };

/** The whole sky in a corner of the zoomed map, a cyan box around what the map shows. */
export function SectorMinimap({
  view,
  stars,
}: {
  view: View;
  /** The stars on the whole sky, in pixels of the mini-map. */
  stars: readonly MinimapStar[];
}) {
  const planet = toMinimapPixels(PLANET_CENTER);
  const frame = minimapFrame(view);
  return (
    <CutFrame
      data-minimap
      aria-hidden="true"
      corners={6}
      className="absolute top-63 left-172 z-2 h-15 w-36 bg-deep-blue"
      innerClassName="overflow-hidden bg-ink"
    >
      <span
        className="absolute rounded-full bg-blue"
        style={{
          left: planet.x - PLANET_DIAMETER / 2,
          top: planet.y - PLANET_DIAMETER / 2,
          width: PLANET_DIAMETER,
          height: PLANET_DIAMETER,
        }}
      />
      {stars.map(star => (
        <span
          key={star.id}
          className="absolute size-0.5 bg-pale-blue"
          style={{ left: star.place.x, top: star.place.y }}
        />
      ))}
      <span
        className="absolute box-border border-2 border-hud-cyan"
        style={frame}
      />
    </CutFrame>
  );
}
