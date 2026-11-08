/**
 * The systems the client is packaged for. Written apart from the release
 * schema so a component of the browser can list them without loading zod.
 */
export const SYSTEMS = ['linux', 'windows', 'macos'] as const;

export type System = (typeof SYSTEMS)[number];
