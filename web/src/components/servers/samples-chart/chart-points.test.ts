import { describe, expect, it } from 'vitest';

import { areaPoints, CHART_BOX, heightOf, stepPoints } from './chart-points';

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

describe('heightOf', () => {
  it('draws 0 on the floor of the box, the maximum at its top and half of it halfway', () => {
    expect(heightOf(0, 100)).toBe(CHART_BOX.bottom);
    expect(heightOf(100, 100)).toBe(CHART_BOX.top);
    expect(heightOf(50, 100)).toBe((CHART_BOX.top + CHART_BOX.bottom) / 2);
  });
});

describe('stepPoints in another box', () => {
  it('draws against the box it is given', () => {
    const box = {
      width: 100,
      height: 50,
      left: 0,
      right: 100,
      top: 10,
      bottom: 40,
    };

    expect(stepPoints([0, 100], 100, box)).toBe('0,40 50,40 50,10 100,10');
    expect(areaPoints('0,40 100,10', box)).toBe('0,40 0,40 100,10 100,40');
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
