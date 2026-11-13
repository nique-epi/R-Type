import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { heightOf } from '@/components/servers/samples-chart/chart-points';
import { CURVE_BOX, LoadCurve } from './load-curve';

describe('LoadCurve', () => {
  it('names what it draws and when its samples start and end', () => {
    render(
      <LoadCurve
        percents={[20, 94]}
        threshold={90}
        startLabel="il y a 10 min"
        endLabel="maintenant"
        aria-label="Processeur de Paris 1, il y a 10 min"
      />,
    );

    expect(
      screen.getByRole('img', { name: 'Processeur de Paris 1, il y a 10 min' }),
    ).toBeDefined();
    expect(screen.getByText('il y a 10 min')).toBeDefined();
    expect(screen.getByText('maintenant')).toBeDefined();
  });

  it('draws the rule of the threshold at its height', () => {
    const { container } = render(
      <LoadCurve
        percents={[20, 94]}
        threshold={90}
        startLabel="il y a 10 min"
        endLabel="maintenant"
        aria-label="Processeur"
      />,
    );

    expect(container.querySelector('line')?.getAttribute('y1')).toBe(
      String(heightOf(90, 100, CURVE_BOX)),
    );
  });
});
