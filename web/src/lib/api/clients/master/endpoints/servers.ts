import type { FetchSchemaRoutes } from '@better-fetch/fetch';
import { z } from 'zod';

import {
  createdServerSchema,
  gamesSchema,
  MAXIMUM_PAGE_SIZE,
  samplesSchema,
  serversPageSchema,
  statusesSchema,
} from '../../../schemas/master';

/** A page of a list: `limit` from 1 to 100, `offset` from 0. */
const pageQuerySchema = z
  .object({
    limit: z.number().int().min(1).max(MAXIMUM_PAGE_SIZE).optional(),
    offset: z.number().int().min(0).optional(),
  })
  .optional();

export const serversEndpointsSchema = {
  '@get/servers': { query: pageQuerySchema, output: serversPageSchema },
  '@post/servers': {
    input: z.object({ name: z.string() }),
    output: createdServerSchema,
  },
  '@get/servers/:id/games': { output: gamesSchema },
  '@get/servers/:id/samples': { query: pageQuerySchema, output: samplesSchema },
  '@get/servers/:id/statuses': {
    query: pageQuerySchema,
    output: statusesSchema,
  },
} satisfies FetchSchemaRoutes;
