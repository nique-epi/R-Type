import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { PlayerSlots } from './player-slots';

describe('PlayerSlots', () => {
  it('draws the ship of each pilot, named, and the word of each free seat', () => {
    render(
      <PlayerSlots
        freeLabel="Libre"
        seats={[
          { player: 1, title: 'Pilote 1' },
          { player: 2, title: 'Pilote 2' },
          null,
          null,
        ]}
      />,
    );

    expect(
      screen.getAllByRole('img').map(ship => ship.getAttribute('aria-label')),
    ).toEqual(['Pilote 1', 'Pilote 2']);
    expect(screen.getAllByText('Libre')).toHaveLength(2);
  });

  it('keeps the word of a free seat for screen readers in the small slots', () => {
    render(<PlayerSlots size="sm" freeLabel="Libre" seats={[null]} />);

    expect(screen.getByText('Libre').className).toContain('sr-only');
  });
});
