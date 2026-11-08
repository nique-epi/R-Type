/** The box of the chart, in units of its view box: 1056 × 240. */
export const CHART_BOX = {
  width: 1056,
  height: 240,
  left: 40,
  right: 1040,
  top: 16,
  bottom: 208,
} as const;

/**
 * The stepped line of the samples, left to right across the chart: each
 * sample holds its value until the next one, as a count of pilots does.
 * A value is drawn against `maximum`; one above it is drawn at the top.
 */
export function stepPoints(
  samples: readonly number[],
  maximum: number,
): string {
  if (samples.length === 0 || maximum <= 0) {
    return '';
  }
  const step = (CHART_BOX.right - CHART_BOX.left) / samples.length;
  const yOf = (value: number) =>
    CHART_BOX.bottom -
    (Math.min(value, maximum) / maximum) * (CHART_BOX.bottom - CHART_BOX.top);
  const points: string[] = [];
  samples.forEach((value, index) => {
    const y = Math.round(yOf(value));
    points.push(`${Math.round(CHART_BOX.left + index * step)},${y}`);
    points.push(`${Math.round(CHART_BOX.left + (index + 1) * step)},${y}`);
  });
  return points.join(' ');
}

/** The area under the line, closed along the bottom of the chart. */
export function areaPoints(line: string): string {
  if (line === '') {
    return '';
  }
  return `${CHART_BOX.left},${CHART_BOX.bottom} ${line} ${CHART_BOX.right},${CHART_BOX.bottom}`;
}
