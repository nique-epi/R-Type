import { describe, expect, it } from 'vitest';
import { gsap, ScrollTrigger } from '@/lib/gsap';

/** `globals()` exists at run time, but the types of gsap do not declare it. */
interface GsapCoreWithGlobals {
  globals(): Record<string, unknown>;
}

describe('lib/gsap', () => {
  it('registers ScrollTrigger on the shared gsap instance', () => {
    const core = gsap.core as unknown as GsapCoreWithGlobals;

    expect(core.globals().ScrollTrigger).toBe(ScrollTrigger);
  });
});
