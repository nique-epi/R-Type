import { cn } from '@/lib/utils';
import { MAP_PIXELS, type Point } from './geometry';
import type { StarMark, StarTone } from './rules';

const TONE_COLORS: Record<StarTone, string> = {
  lit: 'text-star-white',
  slow: 'text-warning-orange',
  dim: 'text-blue',
};

/** A cross is two bars 6 pixels thick; a crowd is a larger cross. */
const MARK_CLASSES: Record<StarMark, string> = {
  cross:
    'size-2.5 before:absolute before:top-0 before:left-[calc(50%-3px)] before:h-full before:w-1.5 before:bg-current after:absolute after:top-[calc(50%-3px)] after:left-0 after:h-1.5 after:w-full after:bg-current',
  crowd:
    'size-3.5 before:absolute before:top-0 before:left-[calc(50%-3px)] before:h-full before:w-1.5 before:bg-current after:absolute after:top-[calc(50%-3px)] after:left-0 after:h-1.5 after:w-full after:bg-current',
  empty: 'size-1.5 bg-current',
  off: 'box-border size-2.5 border-2 border-current',
};

/** A name on the right half of the map is written on the left of its star. */
const NAME_FLIP_X = 600;
const NAME_GAP = 14;
const NAME_RISE = 10;

export type SectorStarProps = {
  place: Point;
  name: string;
  mark: StarMark;
  tone: StarTone;
  chosen: boolean;
  'aria-label': string;
  onPick: () => void;
};

/** A server on the map: a mark the reader clicks, and its name beside it. */
export function SectorStar({
  place,
  name,
  mark,
  tone,
  chosen,
  'aria-label': label,
  onPick,
}: SectorStarProps) {
  const nameOnLeft = place.x > NAME_FLIP_X;
  return (
    <>
      <button
        type="button"
        data-star
        aria-label={label}
        aria-pressed={chosen}
        onClick={onPick}
        className="absolute -mt-3.5 -ml-3.5 grid size-7 cursor-pointer place-items-center"
        style={{ left: place.x, top: place.y }}
      >
        <span
          className={cn(
            'relative block',
            MARK_CLASSES[mark],
            TONE_COLORS[tone],
          )}
        />
      </button>
      <span
        data-star-name
        aria-hidden="true"
        className={cn(
          'text-outline-ink pointer-events-none absolute whitespace-nowrap small-caps',
          chosen
            ? 'text-hud-cyan'
            : tone === 'dim'
              ? 'text-blue'
              : 'text-pale-blue',
        )}
        style={{
          top: place.y - NAME_RISE,
          ...(nameOnLeft
            ? { right: MAP_PIXELS.width - place.x + NAME_GAP }
            : { left: place.x + NAME_GAP }),
        }}
      >
        {name}
      </span>
    </>
  );
}

/** The four corner brackets that lock onto the chosen star. */
export function StarLock({ place }: { place: Point }) {
  return (
    <span
      data-star-lock
      aria-hidden="true"
      className="pointer-events-none absolute -mt-[15px] -ml-[15px] size-[30px] bg-no-repeat [--lock:linear-gradient(var(--color-hud-cyan),var(--color-hud-cyan))] [background-image:var(--lock),var(--lock),var(--lock),var(--lock),var(--lock),var(--lock),var(--lock),var(--lock)] [background-position:0_0,0_0,100%_0,100%_0,0_100%,0_100%,100%_100%,100%_100%] [background-size:8px_2px,2px_8px,8px_2px,2px_8px,8px_2px,2px_8px,8px_2px,2px_8px]"
      style={{ left: place.x, top: place.y }}
    />
  );
}
