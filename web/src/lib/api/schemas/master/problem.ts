import { z } from 'zod';

/** The problem document of RFC 9457 every error of the master carries. */
export const problemSchema = z.object({
  status: z.number(),
  title: z.string(),
  detail: z.string().optional(),
});
