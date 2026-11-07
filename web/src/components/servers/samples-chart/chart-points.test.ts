import { describe, expect, it } from 'vitest';

import { areaPoints, CHART_BOX, stepPoints } from './chart-points';

describe('stepPoints', () => {
  it('draws nothing without samples, or against a maximum of 0', () => {
    expect(stepPoints([], 10)).toBe('');
    expect(stepPoints([3], 0)).toBe('');
  });

  it('holds each sample across its share of the width, 0 on the floor and the maximum at the top', () => {
    expect(stepPoints([0, 10], 10)).toBe(
      `${CHART_BOX.left},${CHART_BOX.bottom} 540,${CHART_BOX.bottom} 540,${CHART_BOX.top} ${CHART_BOX.right},${CHART_BOX.top}`,
    );
  });

  it('draws a value above the maximum at the top', () => {
    expect(stepPoints([25], 10)).toBe(
      `${CHART_BOX.left},${CHART_BOX.top} ${CHART_BOX.right},${CHART_BOX.top}`,
    );
  });
});

describe('areaPoints', () => {
  it('closes the line along the floor', () => {
    expect(areaPoints('40,16 1040,16')).toBe(
      `${CHART_BOX.left},${CHART_BOX.bottom} 40,16 1040,16 ${CHART_BOX.right},${CHART_BOX.bottom}`,
    );
  });

  it('draws no area without a line', () => {
    expect(areaPoints('')).toBe('');
  });
});
