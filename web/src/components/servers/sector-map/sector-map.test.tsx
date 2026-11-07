import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { SectorMap } from './sector-map';
import type { MapServer } from './types';

const NEAR: MapServer = {
  id: 'nebula-lyon',
  name: 'Nebula Lyon',
  status: 'online',
  version: 'v0.1.0',
  compatible: true,
  number: 3,
  ping: 18,
  capacity: 32,
  games: [
    { pilots: 4, state: 'playing' },
    { pilots: 2, state: 'waiting' },
  ],
};

const FAR: MapServer = {
  ...NEAR,
  id: 'orbit-night',
  name: 'Orbit Night',
  number: 21,
  ping: 150,
  games: [{ pilots: 1, state: 'waiting' }],
};

function renderMap(readOnly = false) {
  const callbacks = {
    onJoin: vi.fn(),
    onLeave: vi.fn(),
    onOpenServer: vi.fn(),
    onLeaderboard: vi.fn(),
    onHost: vi.fn(),
    onExit: vi.fn(),
  };
  render(
    <SectorMap
      servers={[FAR, NEAR]}
      labels={fr.sectorMap}
      locale="fr"
      {...callbacks}
      {...(readOnly ? { onJoin: undefined, onLeave: undefined } : {})}
    />,
  );
  const map = screen.getByRole('application', { name: fr.sectorMap.screen });
  const press = (key: string) => fireEvent.keyDown(map, { key });
  return { callbacks, press };
}

describe('SectorMap', () => {
  it('chooses the nearest server on the first Enter, then zooms on it', () => {
    const { press } = renderMap();

    press('Enter');
    expect(screen.getByRole('status').textContent).toContain(
      fr.sectorMap.bubbleZoom,
    );
    expect(screen.getByRole('status').textContent).toContain('Nebula Lyon');

    press('Enter');
    expect(screen.getByRole('status').textContent).toContain(
      fr.sectorMap.bubbleGames,
    );
  });

  it('lists the games on the third Enter, and joins the waiting one with Enter once it is chosen', () => {
    const { press, callbacks } = renderMap();

    press('Enter');
    press('Enter');
    press('Enter');
    expect(
      screen.getByRole('dialog', { name: 'Parties de Nebula Lyon' }),
    ).toBeDefined();

    press('Enter');
    expect(callbacks.onJoin).not.toHaveBeenCalled();

    press('ArrowDown');
    press('Enter');
    expect(callbacks.onJoin).toHaveBeenCalledWith('nebula-lyon', 1);
  });

  it('steps back one stage on each Escape, then leaves the map', () => {
    const { press, callbacks } = renderMap();

    press('Enter');
    press('Enter');
    press('Escape');
    expect(callbacks.onExit).not.toHaveBeenCalled();
    expect(screen.getByRole('status').textContent).toContain(
      fr.sectorMap.bubbleZoom,
    );

    press('Escape');
    expect(callbacks.onExit).toHaveBeenCalledOnce();
  });

  it('joins the nearest free seat on Q', () => {
    const { press, callbacks } = renderMap();

    press('q');

    expect(callbacks.onJoin).toHaveBeenCalledWith('nebula-lyon', 1);
  });

  it('counts the servers it draws in its subtitle', () => {
    renderMap();

    expect(screen.getByText(/^2 SERVEURS - /)).toBeDefined();
  });

  describe('without a way to join', () => {
    it('offers no quick play, and Q does nothing', () => {
      const { press, callbacks } = renderMap(true);

      press('q');

      expect(screen.queryByText(fr.sectorMap.quickPlay)).toBeNull();
      expect(callbacks.onJoin).not.toHaveBeenCalled();
      expect(screen.queryByRole('dialog')).toBeNull();
    });

    it('lists the games without a Join chip, and keeps the sheet, the games and Back', () => {
      const { press } = renderMap(true);

      press('Enter');
      press('Enter');
      press('Enter');

      expect(
        screen.getByRole('dialog', { name: 'Parties de Nebula Lyon' }),
      ).toBeDefined();
      expect(screen.queryByText(fr.sectorMap.join)).toBeNull();
      expect(screen.queryByText(fr.sectorMap.leave)).toBeNull();
      expect(screen.getByText(fr.sectorMap.sheet)).toBeDefined();
      expect(screen.getByText(fr.sectorMap.back)).toBeDefined();
    });

    it('says nothing under the games about a join Enter cannot do', () => {
      const { press } = renderMap(true);

      press('Enter');
      press('Enter');
      press('Enter');
      press('ArrowDown');

      expect(screen.queryByText(/ENTRÉE POUR REJOINDRE/)).toBeNull();
    });
  });
});
