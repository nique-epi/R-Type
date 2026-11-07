import { cleanup } from '@testing-library/react';
import { afterEach } from 'vitest';

/**
 * Testing Library unmounts what a test rendered only when the runner
 * exposes `afterEach` as a global; vitest does not here, so each test
 * would see the screens of the tests before it.
 */
afterEach(() => {
  cleanup();
});
