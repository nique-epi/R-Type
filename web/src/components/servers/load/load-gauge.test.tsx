import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { LoadGauge, litSegmentsOf } from './load-gauge';

describe('litSegmentsOf', () => {
  it('lights one segment per ten percent, to the nearest', () => {
    expect(litSegmentsOf(0)).toBe(0);
    expect(litSegmentsOf(4)).toBe(0);
    expect(litSegmentsOf(41)).toBe(4);
    expect(litSegmentsOf(94)).toBe(9);
    expect(litSegmentsOf(95)).toBe(10);
    expect(litSegmentsOf(100)).toBe(10);
  });

  it('stays within the ten segments for a percent outside 0 to 100', () => {
    expect(litSegmentsOf(-5)).toBe(0);
    expect(litSegmentsOf(140)).toBe(10);
  });
});

describe('LoadGauge', () => {
  it('writes the percent, what it is a share of and the level in words', () => {
    render(
      <LoadGauge
        label="Mémoire"
        reading={{
          percent: 94,
          text: '94 %',
          level: 'flatOut',
          levelWord: 'à fond',
          detail: '7,5 / 8 Go',
        }}
        notMeasured="non mesurée"
      />,
    );

    expect(screen.getByText('94 %')).toBeDefined();
    expect(screen.getByText('à fond')).toBeDefined();
    expect(screen.getByText(/7,5 \/ 8 Go/)).toBeDefined();
    expect(screen.getByRole('meter').getAttribute('value')).toBe('94');
    expect(screen.queryByText(/non mesurée/)).toBeNull();
  });

  it('says a server took no measure, with no meter and no percent', () => {
    render(
      <LoadGauge label="Processeur" reading={null} notMeasured="non mesuré" />,
    );

    expect(screen.getByText(/non mesuré/)).toBeDefined();
    expect(screen.queryByRole('meter')).toBeNull();
    expect(screen.queryByText(/%/)).toBeNull();
  });

  it('writes 0 % for a machine at rest, which is a measure', () => {
    render(
      <LoadGauge
        label="Processeur"
        reading={{
          percent: 0,
          text: '0 %',
          level: 'calm',
          levelWord: "à l'aise",
        }}
        notMeasured="non mesuré"
      />,
    );

    expect(screen.getByText('0 %')).toBeDefined();
    expect(screen.getByRole('meter').getAttribute('value')).toBe('0');
  });
});
