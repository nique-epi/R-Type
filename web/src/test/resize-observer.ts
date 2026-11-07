/**
 * jsdom has no `ResizeObserver`, which the game screens use to fit their
 * box. This one never reports a change, as a box that keeps its size.
 */
class StillResizeObserver {
  observe() {}
  unobserve() {}
  disconnect() {}
}

window.ResizeObserver = StillResizeObserver;
