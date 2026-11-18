import type { FetchSchemaRoutes } from '@better-fetch/fetch';
import { z } from 'zod';

import {
  createdSessionSchema,
  deviceLinkDecisionSchema,
  deviceLinkStateSchema,
} from '../../../schemas/master';

export const sessionsEndpointsSchema = {
  '@post/sessions': {
    input: z.object({ nickname: z.string(), password: z.string() }),
    output: createdSessionSchema,
  },
  '@delete/sessions/me': { output: z.unknown() },
  '@get/device-links/:code': { output: deviceLinkStateSchema },
  '@put/device-links/:code/decision': {
    input: z.object({ decision: deviceLinkDecisionSchema }),
    output: deviceLinkStateSchema,
  },
} satisfies FetchSchemaRoutes;
