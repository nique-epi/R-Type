/** The box of a chart, in units of its view box: its size and where its line may go. */
export type ChartBox = {
  width: number;
  height: number;
  left: number;
  right: number;
  top: number;
  bottom: number;
};

/** The box of the chart, in units of its view box: 1056 × 240. */
export const CHART_BOX = {
  width: 1056,
  height: 240,
  left: 40,
  right: 1040,
  top: 16,
  bottom: 208,
} as const satisfies ChartBox;

/** The height, in `box`, `value` is drawn at against `maximum`; a value above it is drawn at the top. */
export function heightOf(
  value: number,
  maximum: number,
  box: ChartBox = CHART_BOX,
): number {
  return Math.round(
    box.bottom - (Math.min(value, maximum) / maximum) * (box.bottom - box.top),
  );
}

/**
 * The stepped line of the samples, left to right across `box`: each
 * sample holds its value until the next one, as a count of pilots does.
 * A value is drawn against `maximum`; one above it is drawn at the top.
 */
export function stepPoints(
  samples: readonly number[],
  maximum: number,
  box: ChartBox = CHART_BOX,
): string {
  if (samples.length === 0 || maximum <= 0) {
    return '';
  }
  const step = (box.right - box.left) / samples.length;
  const points: string[] = [];
  samples.forEach((value, index) => {
    const y = heightOf(value, maximum, box);
    points.push(`${Math.round(box.left + index * step)},${y}`);
    points.push(`${Math.round(box.left + (index + 1) * step)},${y}`);
  });
  return points.join(' ');
}

/** The area under the line, closed along the bottom of `box`. */
export function areaPoints(line: string, box: ChartBox = CHART_BOX): string {
  if (line === '') {
    return '';
  }
  return `${box.left},${box.bottom} ${line} ${box.right},${box.bottom}`;
}
