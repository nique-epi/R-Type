import { z } from 'zod';

const DEFAULT_MASTER_URL = 'http://localhost:8080';

const envSchema = z.object({
  MASTER_URL: z
    .url({
      protocol: /^https?$/,
      error: 'MASTER_URL must be the http(s) URL of a running master',
    })
    .default(DEFAULT_MASTER_URL),
});

export type Env = z.infer<typeof envSchema>;

/** Validates the variables `next dev` reads, so a wrong one stops it at start. */
export function parseEnv(): Env {
  const result = envSchema.safeParse(process.env);

  if (!result.success) {
    throw new Error(
      `Invalid environment variables:\n${z.prettifyError(result.error)}`,
    );
  }

  return result.data;
}
