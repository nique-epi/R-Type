import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { GamesTable } from './games-table';

const FREE = { player: 1 as const, title: 'Pilote 1' };

const WAITING = {
  id: 'game-1',
  label: 'Partie 1',
  state: 'waiting' as const,
  stateLabel: 'En attente',
  seats: [FREE, null, null, null],
};

describe('GamesTable', () => {
  it('is read only: one column for the game, its state and its seats, and no button', () => {
    render(<GamesTable rows={[WAITING]} labels={fr.gamesTable} />);

    expect(screen.queryByRole('button')).toBeNull();
    expect(screen.getAllByRole('columnheader')).toHaveLength(3);
    expect(screen.getByText('Partie 1')).toBeDefined();
  });
});
