/**
 * The longest name of a server the master takes, in bytes of UTF-8
 * (`MAXIMUM_SERVER_NAME_SIZE` of the protocol): 32 letters without accent.
 */
export const SERVER_NAME_MAXIMUM_BYTES = 32;

/** Why the master would refuse a name. */
export type ServerNameProblem = 'empty' | 'tooLong';

const encoder = new TextEncoder();

/** The size of a name as the master counts it: its bytes of UTF-8. */
export function serverNameSizeOf(name: string): number {
  return encoder.encode(name).length;
}

/**
 * What is wrong with the name of a server, the blanks around it left out, or
 * `null` for a name the master takes. The form and the server action both ask,
 * so a name refused here never reaches the master.
 */
export function serverNameProblemOf(name: string): ServerNameProblem | null {
  const trimmed = name.trim();

  if (trimmed === '') {
    return 'empty';
  }

  return serverNameSizeOf(trimmed) > SERVER_NAME_MAXIMUM_BYTES
    ? 'tooLong'
    : null;
}
