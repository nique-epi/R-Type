import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { ServerLoad } from './server-load';

const UNMEASURED = { latest: null, history: [] };

describe('ServerLoad', () => {
  it('writes the load of the newest sample, its level and the memory in gigabytes, in French', () => {
    render(
      <ServerLoad
        lang="fr"
        name="Paris 1"
        span={600}
        load={{
          cpu: { latest: 94, history: [20, 94] },
          memory: { latest: 41, history: [41] },
          memoryBytes: { used: 3_300_000_000, total: 8_000_000_000 },
        }}
      />,
    );

    expect(screen.getByText('94 %')).toBeDefined();
    expect(screen.getByText('à fond')).toBeDefined();
    expect(screen.getByText('41 %')).toBeDefined();
    expect(screen.getByText("à l'aise")).toBeDefined();
    expect(screen.getByText(/3,3 \/ 8 Go/)).toBeDefined();
  });

  it('draws a curve for a load of two samples and none for a single one', () => {
    render(
      <ServerLoad
        lang="en"
        name="Paris 1"
        span={600}
        load={{
          cpu: { latest: 94, history: [20, 94] },
          memory: { latest: 41, history: [41] },
          memoryBytes: { used: 3_300_000_000, total: 8_000_000_000 },
        }}
      />,
    );

    expect(screen.getAllByRole('img')).toHaveLength(1);
    expect(
      screen.getByRole('img', { name: /Processor of Paris 1/ }),
    ).toBeDefined();
  });

  it('says not measured, and no percent, for a server that sends no load', () => {
    render(
      <ServerLoad
        lang="fr"
        name="Paris 1"
        span={0}
        load={{ cpu: UNMEASURED, memory: UNMEASURED, memoryBytes: null }}
      />,
    );

    expect(screen.getByText(/non mesuré$/)).toBeDefined();
    expect(screen.getByText(/non mesurée$/)).toBeDefined();
    expect(screen.queryByText(/%/)).toBeNull();
    expect(screen.queryByRole('meter')).toBeNull();
  });
});
