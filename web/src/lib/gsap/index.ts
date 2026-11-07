import { useGSAP } from '@gsap/react';
import gsap from 'gsap';
import { ScrollTrigger } from 'gsap/ScrollTrigger';

gsap.registerPlugin(useGSAP, ScrollTrigger);

/**
 * The media query under which the site animates. A reader who asked their
 * system for reduced motion gets every element in its final place, still.
 */
export const MOTION_ALLOWED = '(prefers-reduced-motion: no-preference)';

export { gsap, ScrollTrigger, useGSAP };
