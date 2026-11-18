import { z } from 'zod';

import { userSchema } from './users';

/**
 * The session `POST /api/sessions` opens: its token, shown this once, the
 * second it expires (in seconds since 1970) and the account it belongs to.
 */
export const createdSessionSchema = z.object({
  token: z.string(),
  expiresAt: z.number().int(),
  account: userSchema,
});

export type CreatedSession = z.infer<typeof createdSessionSchema>;

/** Where a device link stands. */
export const deviceLinkStatusSchema = z.enum([
  'pending',
  'approved',
  'denied',
  'expired',
]);

export type DeviceLinkStatus = z.infer<typeof deviceLinkStatusSchema>;

/** What a player decides on a device link. */
export const deviceLinkDecisionSchema = z.enum(['approved', 'denied']);

export type DeviceLinkDecision = z.infer<typeof deviceLinkDecisionSchema>;

/**
 * A device link as `GET /api/device-links/{code}` shows it: its short code,
 * where it stands, when the game asked for it and when it expires (in seconds
 * since 1970). It never holds the secret of the game.
 */
export const deviceLinkStateSchema = z.object({
  userCode: z.string(),
  status: deviceLinkStatusSchema,
  createdAt: z.number().int(),
  expiresAt: z.number().int(),
  interval: z.number().int(),
});

export type DeviceLinkState = z.infer<typeof deviceLinkStateSchema>;
