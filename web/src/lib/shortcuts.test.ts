import { describe, expect, it, vi } from 'vitest';

import { runShortcut } from './shortcuts';

function press(key: string) {
  return { key, preventDefault: vi.fn() };
}

describe('runShortcut', () => {
  it('runs the action of the key and keeps the browser from using it', () => {
    const filter = vi.fn();
    const event = press('f');

    expect(runShortcut(event, { f: filter })).toBe(true);
    expect(filter).toHaveBeenCalledOnce();
    expect(event.preventDefault).toHaveBeenCalledOnce();
  });

  it('runs the action of a letter typed with Shift or Caps Lock', () => {
    const quickPlay = vi.fn();

    runShortcut(press('Q'), { q: quickPlay });

    expect(quickPlay).toHaveBeenCalledOnce();
  });

  it('leaves a key without a shortcut to the browser', () => {
    const event = press('Tab');

    expect(runShortcut(event, { q: vi.fn() })).toBe(false);
    expect(event.preventDefault).not.toHaveBeenCalled();
  });
});
