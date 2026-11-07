import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it, vi } from 'vitest';

import { Tabs } from './tabs';

const SYSTEMS = [
  { value: 'linux', label: 'Linux' },
  { value: 'windows', label: 'Windows' },
  { value: 'macos', label: 'macOS' },
] as const;

function renderTabs(value: (typeof SYSTEMS)[number]['value']) {
  const onValueChange = vi.fn();
  render(
    <Tabs
      aria-label="Système"
      items={SYSTEMS}
      value={value}
      onValueChange={onValueChange}
    />,
  );
  return onValueChange;
}

describe('Tabs', () => {
  it('puts only the open tab in the Tab order', () => {
    renderTabs('windows');

    expect(screen.getByRole('tab', { name: 'Windows' }).tabIndex).toBe(0);
    expect(screen.getByRole('tab', { name: 'Linux' }).tabIndex).toBe(-1);
    expect(
      screen
        .getByRole('tab', { name: 'Windows' })
        .getAttribute('aria-selected'),
    ).toBe('true');
  });

  it('opens the next tab on ArrowRight, and wraps from the last to the first', () => {
    const onValueChange = renderTabs('macos');

    fireEvent.keyDown(screen.getByRole('tab', { name: 'macOS' }), {
      key: 'ArrowRight',
    });

    expect(onValueChange).toHaveBeenCalledWith('linux');
  });

  it('opens the previous tab on ArrowLeft, and wraps from the first to the last', () => {
    const onValueChange = renderTabs('linux');

    fireEvent.keyDown(screen.getByRole('tab', { name: 'Linux' }), {
      key: 'ArrowLeft',
    });

    expect(onValueChange).toHaveBeenCalledWith('macos');
  });

  it('goes to the ends on Home and End', () => {
    const onValueChange = renderTabs('windows');
    const open = screen.getByRole('tab', { name: 'Windows' });

    fireEvent.keyDown(open, { key: 'End' });
    fireEvent.keyDown(open, { key: 'Home' });

    expect(onValueChange.mock.calls).toEqual([['macos'], ['linux']]);
  });
});
