import 'server-only';

import { createFetch, createSchema } from '@better-fetch/fetch';
import { env } from '@/config/server-env';

import { problemSchema } from '../../schemas/master';
import { healthEndpointsSchema } from './endpoints';

const MASTER_API_PREFIX = '/api';

const masterSchema = createSchema({
  ...healthEndpointsSchema,
});

export const master = createFetch({
  baseURL: `${env.MASTER_URL}${MASTER_API_PREFIX}`,
  schema: masterSchema,
  defaultError: problemSchema,
  catchAllError: true,
});

export const MASTER_ERROR_CODES = {
  400: 'BAD_REQUEST',
  401: 'UNAUTHORIZED',
  404: 'NOT_FOUND',
  409: 'CONFLICT',
  413: 'PAYLOAD_TOO_LARGE',
  429: 'TOO_MANY_REQUESTS',
  500: 'INTERNAL_SERVER_ERROR',
} as const;

export type MasterErrorCode =
  (typeof MASTER_ERROR_CODES)[keyof typeof MASTER_ERROR_CODES];
