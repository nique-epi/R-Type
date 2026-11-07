import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

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
  it('has an action column and a button when a row offers one', () => {
    const join = vi.fn();
    render(
      <GamesTable
        rows={[
          { ...WAITING, actionLabel: 'Rejoindre', onJoin: join },
          { ...WAITING, id: 'game-2', actionLabel: 'Rejoindre' },
        ]}
        labels={fr.gamesTable}
      />,
    );

    const buttons = screen.getAllByRole('button', { name: 'Rejoindre' });
    expect(buttons).toHaveLength(2);
    fireEvent.click(buttons[0] as HTMLElement);
    expect(join).toHaveBeenCalledOnce();
    expect(screen.getAllByRole('columnheader')).toHaveLength(4);
  });

  it('is read only, with no action column, when no row has a label for one', () => {
    render(<GamesTable rows={[WAITING]} labels={fr.gamesTable} />);

    expect(screen.queryByRole('button')).toBeNull();
    expect(screen.getAllByRole('columnheader')).toHaveLength(3);
    expect(screen.getByText('Partie 1')).toBeDefined();
  });
});
