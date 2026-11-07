/**
 * jsdom has no `window.matchMedia`, which ScrollTrigger calls when it is
 * registered. Every query matches nothing, as in a browser that satisfies none
 * of them.
 */
window.matchMedia = (query: string): MediaQueryList => ({
  matches: false,
  media: query,
  onchange: null,
  addEventListener: () => {},
  removeEventListener: () => {},
  addListener: () => {},
  removeListener: () => {},
  dispatchEvent: () => false,
});
