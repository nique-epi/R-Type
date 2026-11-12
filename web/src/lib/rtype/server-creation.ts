/** A server the master just created, with the key it shows this once. */
export type CreatedServer = {
  id: number;
  name: string;
  key: string;
};

/**
 * How the creation of a server ended, as the hosting page reads it: the
 * server and its key, or why there is none. `invalidName` never reached the
 * master, `quota` is its 429, `refused` a name it does not take, and
 * `unreachable` a master that did not answer or answered something else.
 */
export type ServerCreationOutcome =
  | { status: 'created'; server: CreatedServer }
  | { status: 'invalidName' | 'quota' | 'refused' | 'unreachable' };

/** Creates a server named `name`; the server action of the hosting page. */
export type CreateServer = (name: string) => Promise<ServerCreationOutcome>;

/**
 * The lines a host pastes in the `server.cfg` of a server to list it on the
 * master at `masterUrl` with `key`, as `server.cfg.example` writes them.
 */
export function serverConfigLinesOf(masterUrl: string, key: string): string[] {
  return [`set master-url ${masterUrl}`, `set master-key "${key}"`];
}
