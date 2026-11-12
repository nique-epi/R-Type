import { z } from 'zod';

/** The most items one page of a list holds (`MAXIMUM_PAGE_SIZE` of the master). */
export const MAXIMUM_PAGE_SIZE = 100;

/**
 * The statuses the master gives a game server, in the order it applies them
 * (docs "Statuses of the game servers").
 */
export const masterStatusSchema = z.enum([
  'maintenance',
  'offline',
  'unreachable',
  'degraded',
  'online',
]);

export type MasterStatus = z.infer<typeof masterStatusSchema>;

/** A game server as `GET /api/servers` lists it. */
export const listedServerSchema = z.object({
  id: z.number().int(),
  name: z.string(),
  gameVersion: z.string(),
  maximumGames: z.number().int(),
  maximumPlayersPerGame: z.number().int(),
  status: masterStatusSchema,
  gameCount: z.number().int(),
  playerCount: z.number().int(),
});

export type ListedServer = z.infer<typeof listedServerSchema>;

export const serversPageSchema = z.object({
  servers: z.array(listedServerSchema),
});

/** The shape of a key of a server: `rtk_`, then 64 hexadecimal digits. */
const SERVER_KEY_PATTERN = /^rtk_[0-9a-f]{64}$/;

/** The server `POST /api/servers` creates, with the key shown this once. */
export const createdServerSchema = z.object({
  id: z.number().int(),
  name: z.string(),
  key: z.string().regex(SERVER_KEY_PATTERN),
});

export type CreatedServer = z.infer<typeof createdServerSchema>;

/** The state of a game as the master folds it for a player who picks one. */
export const masterGameStateSchema = z.enum(['waiting', 'full', 'inGame']);

export const masterGameSchema = z.object({
  number: z.number().int(),
  name: z.string(),
  state: masterGameStateSchema,
  playerCount: z.number().int(),
});

export type MasterGame = z.infer<typeof masterGameSchema>;

export const gamesSchema = z.object({ games: z.array(masterGameSchema) });

/** One heartbeat of a server kept as a sample; a measure it did not take is absent. */
export const sampleSchema = z.object({
  takenAt: z.number(),
  gameCount: z.number().int(),
  playerCount: z.number().int(),
  measures: z
    .object({
      averagePingMilliseconds: z.number().optional(),
    })
    .optional(),
});

export type Sample = z.infer<typeof sampleSchema>;

export const samplesSchema = z.object({ samples: z.array(sampleSchema) });

export const statusChangeSchema = z.object({
  status: masterStatusSchema,
  since: z.number(),
});

export type StatusChange = z.infer<typeof statusChangeSchema>;

export const statusesSchema = z.object({
  statuses: z.array(statusChangeSchema),
});
