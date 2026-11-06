import type { FetchSchemaRoutes } from '@better-fetch/fetch';

import { healthSchema } from '../../../schemas/master';

export const healthEndpointsSchema = {
  '@get/health': {
    output: healthSchema,
  },
} satisfies FetchSchemaRoutes;
