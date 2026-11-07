import { IBM_Plex_Mono } from 'next/font/google';
import localFont from 'next/font/local';

/**
 * The faces of the site. The two pixel faces are the ones the game client
 * draws with (assets/fonts/), copied beside this site so Next.js can serve
 * them; IBM Plex Mono is the reading face the game has no equivalent of.
 * Each one sets a CSS variable that globals.css turns into a font family.
 */
export const pressStart = localFont({
  src: '../styles/fonts/press_start_2p.ttf',
  variable: '--font-press-start',
  weight: '400',
  display: 'swap',
});

export const silkscreen = localFont({
  src: '../styles/fonts/silkscreen.ttf',
  variable: '--font-silkscreen',
  weight: '400',
  display: 'swap',
});

export const plexMono = IBM_Plex_Mono({
  subsets: ['latin'],
  weight: ['400', '500'],
  variable: '--font-plex-mono',
  display: 'swap',
});

/** The class names that set the three font variables, for the root element. */
export const FONT_VARIABLES = [
  pressStart.variable,
  silkscreen.variable,
  plexMono.variable,
].join(' ');
