import { CutFrame } from '@/components/game/cut-frame';
import { GameText } from '@/components/game/game-text';
import { cn } from '@/lib/utils';
import { MAP_PIXELS, type Point } from './geometry';

/** One line of the bubble and the token it is written in. */
export type BubbleLine = {
  text: string;
  tone: 'plain' | 'warning' | 'hint';
};

const LINE_TONES: Record<BubbleLine['tone'], string> = {
  plain: 'text-pale-blue',
  warning: 'text-warning-orange',
  hint: 'text-hud-cyan',
};

const BUBBLE_WIDTH = 240;
const BUBBLE_GAP = 28;
const TITLE_HEIGHT = 36;
/** A row of text is 16 pixels tall, and each line leaves 4 under it. */
const ROW_HEIGHT = 16;
const LINE_GAP = 4;
const PADDING = 8;
const EDGE = 4;
const RISE = 16;

/**
 * What a row of the bubble holds. Silkscreen at 16 pixels is 10.5 to 11.5
 * pixels a letter, against the 216 pixels of the inside: 19 letters, counted
 * short so a row never holds more than the bubble was made tall for.
 */
const LETTERS_PER_ROW = 19;

/** How many rows a text takes once its words wrap at `LETTERS_PER_ROW`. */
function rowsOfText(text: string): number {
  let rows = 1;
  let used = 0;

  for (const word of text.split(' ')) {
    const needed = used === 0 ? word.length : used + 1 + word.length;
    if (used > 0 && needed > LETTERS_PER_ROW) {
      rows += 1;
      used = word.length;
    } else {
      used = needed;
    }
  }

  return rows;
}

/** How many rows the lines of a bubble take, a line that is too long taking more than one. */
export function rowsOf(lines: readonly BubbleLine[]): number {
  return lines.reduce((rows, { text }) => rows + rowsOfText(text), 0);
}

/** Where the bubble of a star lands: beside it, on the side with room, inside the map. */
export function bubbleBox(
  place: Point,
  lines: readonly BubbleLine[],
): { left: number; top: number; height: number } {
  const height =
    TITLE_HEIGHT +
    rowsOf(lines) * ROW_HEIGHT +
    lines.length * LINE_GAP +
    PADDING;
  const left =
    place.x > MAP_PIXELS.width / 2
      ? place.x - BUBBLE_GAP - BUBBLE_WIDTH
      : place.x + BUBBLE_GAP;
  const top = Math.max(
    EDGE,
    Math.min(place.y - RISE, MAP_PIXELS.height - height - EDGE),
  );
  return { left, top, height };
}

/** The card beside the chosen star: its name, its pilots and ping, what a click does. */
export function SectorBubble({
  place,
  title,
  lines,
}: {
  place: Point;
  title: string;
  lines: readonly BubbleLine[];
}) {
  const box = bubbleBox(place, lines);
  return (
    <CutFrame
      data-bubble
      role="status"
      className="absolute z-3"
      innerClassName="box-border bg-ink px-2.5 pt-2.5"
      style={{
        left: box.left,
        top: box.top,
        width: BUBBLE_WIDTH,
        height: box.height,
      }}
    >
      <GameText className="mb-2 overflow-hidden whitespace-nowrap text-hud-cyan">
        {title}
      </GameText>
      {lines.map(line => (
        <GameText
          key={line.text}
          face="small"
          className={cn('mb-1', LINE_TONES[line.tone])}
        >
          {line.text}
        </GameText>
      ))}
    </CutFrame>
  );
}
