import { createFetch, createSchema } from '@better-fetch/fetch';

import { problemSchema } from '../../schemas/master';
import { healthEndpointsSchema } from './endpoints';

const MASTER_API_BASE_URL = '/api';

const masterSchema = createSchema({
  ...healthEndpointsSchema,
});

export const master = createFetch({
  baseURL: MASTER_API_BASE_URL,
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
