import { render } from '@testing-library/react';
import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest';

import { AutoRefresh } from './auto-refresh';

const router = vi.hoisted(() => ({ refresh: vi.fn() }));

vi.mock('next/navigation', () => ({ useRouter: () => router }));

function showTab(state: 'visible' | 'hidden') {
  Object.defineProperty(document, 'visibilityState', {
    configurable: true,
    value: state,
  });
}

describe('AutoRefresh', () => {
  beforeEach(() => {
    vi.useFakeTimers();
    router.refresh.mockClear();
    showTab('visible');
  });

  afterEach(() => {
    vi.useRealTimers();
  });

  it('reads the page again once each interval, not before', () => {
    render(<AutoRefresh seconds={10} />);

    vi.advanceTimersByTime(9999);
    expect(router.refresh).not.toHaveBeenCalled();

    vi.advanceTimersByTime(1);
    expect(router.refresh).toHaveBeenCalledTimes(1);

    vi.advanceTimersByTime(10000);
    expect(router.refresh).toHaveBeenCalledTimes(2);
  });

  it('waits while its tab is not on screen', () => {
    showTab('hidden');
    render(<AutoRefresh seconds={10} />);

    vi.advanceTimersByTime(30000);

    expect(router.refresh).not.toHaveBeenCalled();
  });

  it('reads again as soon as its tab is shown', () => {
    showTab('hidden');
    render(<AutoRefresh seconds={10} />);

    showTab('visible');
    document.dispatchEvent(new Event('visibilitychange'));

    expect(router.refresh).toHaveBeenCalledTimes(1);
  });

  it('stops when the page goes', () => {
    const { unmount } = render(<AutoRefresh seconds={10} />);

    unmount();
    vi.advanceTimersByTime(60000);
    document.dispatchEvent(new Event('visibilitychange'));

    expect(router.refresh).not.toHaveBeenCalled();
  });
});
