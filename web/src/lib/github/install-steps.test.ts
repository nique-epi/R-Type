import { describe, expect, it } from 'vitest';

import { fr } from '@/i18n/dictionaries/fr';
import { installStepsFor } from './install-steps';

const WORDS = fr.pages.downloadPage.steps;
const APPIMAGE = 'R-Type-client-0.1.0-Linux.AppImage';

describe('installStepsFor', () => {
  it('has three steps on every system', () => {
    for (const system of ['linux', 'windows', 'macos'] as const) {
      expect(installStepsFor(system, WORDS, APPIMAGE)).toHaveLength(3);
    }
  });

  it('makes the AppImage executable, then runs it, by its real file name', () => {
    const steps = installStepsFor('linux', WORDS, APPIMAGE);

    expect(steps[1]?.command).toBe(`chmod +x ${APPIMAGE}`);
    expect(steps[2]?.command).toBe(`./${APPIMAGE}`);
  });

  it('gives Windows and macOS sentences to read, and no command', () => {
    for (const system of ['windows', 'macos'] as const) {
      const steps = installStepsFor(system, WORDS, 'file');

      expect(steps.every(step => step.command === undefined)).toBe(true);
      expect(steps[1]?.text).toBeTruthy();
    }
  });
});
