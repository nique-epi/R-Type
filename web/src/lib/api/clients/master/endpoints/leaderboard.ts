import type { FetchSchemaRoutes } from '@better-fetch/fetch';
import { z } from 'zod';

import {
  accountLeaderboardSchema,
  LEADERBOARD_DIFFICULTIES,
  leaderboardSchema,
  MAXIMUM_LEADERBOARD_PLAYERS,
  MAXIMUM_PAGE_SIZE,
} from '../../../schemas/master';

export const leaderboardEndpointsSchema = {
  '@get/leaderboard': {
    query: z.object({
      difficulty: z.enum(LEADERBOARD_DIFFICULTIES),
      playerCount: z.number().int().min(1).max(MAXIMUM_LEADERBOARD_PLAYERS),
      /** One server only; every server when left out. */
      server: z.number().int().optional(),
      limit: z.number().int().min(1).max(MAXIMUM_PAGE_SIZE).optional(),
      offset: z.number().int().min(0).optional(),
    }),
    output: leaderboardSchema,
  },
  '@get/leaderboard/accounts': {
    query: z.object({
      limit: z.number().int().min(1).max(MAXIMUM_PAGE_SIZE).optional(),
      offset: z.number().int().min(0).optional(),
    }),
    output: accountLeaderboardSchema,
  },
} satisfies FetchSchemaRoutes;
