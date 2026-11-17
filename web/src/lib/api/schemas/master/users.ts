import { z } from 'zod';

/** What a user may do on the master: play, or administer too. */
export const userRoleSchema = z.enum(['player', 'admin']);

/** The account `POST /api/users` creates. */
export const userSchema = z.object({
  id: z.number().int(),
  nickname: z.string(),
  role: userRoleSchema,
});

export type User = z.infer<typeof userSchema>;
