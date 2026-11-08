import type { System } from './systems';

/** The system a browser runs on, read from its user agent; Linux when it names no other. */
export function detectSystem(userAgent: string): System {
  if (/Windows|Win32|Win64/i.test(userAgent)) {
    return 'windows';
  }
  if (/Macintosh|Mac OS X|MacIntel/i.test(userAgent)) {
    return 'macos';
  }
  return 'linux';
}
