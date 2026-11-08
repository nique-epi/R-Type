import { z } from 'zod';

const DEFAULT_MASTER_URL = 'http://localhost:8080';
const DEFAULT_GITHUB_REPOSITORY = 'nique-epi/R-Type';

const envSchema = z.object({
  MASTER_URL: z
    .url({
      protocol: /^https?$/,
      error: 'MASTER_URL must be the http(s) URL of a running master',
    })
    .default(DEFAULT_MASTER_URL),
  GITHUB_REPOSITORY: z
    .string()
    .regex(/^[\w.-]+\/[\w.-]+$/, {
      error: 'GITHUB_REPOSITORY must read owner/name',
    })
    .default(DEFAULT_GITHUB_REPOSITORY),
  GITHUB_TOKEN: z.string().min(1).optional(),
});

type Env = z.infer<typeof envSchema>;

/** Validates the variables of the server, so a wrong one stops it at start. */
export function parseEnv(): Env {
  const result = envSchema.safeParse(process.env);

  if (!result.success) {
    throw new Error(
      `Invalid environment variables:\n${z.prettifyError(result.error)}`,
    );
  }

  return result.data;
}
