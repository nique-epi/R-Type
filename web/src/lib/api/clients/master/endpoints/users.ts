import type { FetchSchemaRoutes } from '@better-fetch/fetch';
import { z } from 'zod';

import { userSchema } from '../../../schemas/master';

export const usersEndpointsSchema = {
  '@post/users': {
    input: z.object({ nickname: z.string(), password: z.string() }),
    output: userSchema,
  },
  '@get/users/me': { output: userSchema },
} satisfies FetchSchemaRoutes;
