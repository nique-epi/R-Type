import { ValidationError } from '@better-fetch/fetch';

import { MASTER_ERROR_CODES } from '../../clients';
import type { ApiErrorCode, ApiResponse } from '../types';

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null;
}

function isMasterErrorStatus(
  status: unknown,
): status is keyof typeof MASTER_ERROR_CODES {
  return typeof status === 'number' && status in MASTER_ERROR_CODES;
}

function hasProblemDocument(error: Record<string, unknown>): boolean {
  return typeof error.title === 'string';
}

function toApiErrorCode(error: unknown): ApiErrorCode {
  if (!isRecord(error)) {
    return 'UNKNOWN_ERROR';
  }

  if (error.error instanceof ValidationError) {
    return 'ERR_API_VALIDATION_FAILED';
  }

  if (error.error instanceof Error) {
    return 'ERR_API_FETCH_FAILED';
  }

  if (!isMasterErrorStatus(error.status)) {
    return 'UNKNOWN_ERROR';
  }

  return hasProblemDocument(error)
    ? MASTER_ERROR_CODES[error.status]
    : 'ERR_API_FETCH_FAILED';
}

/** Maps a better-fetch error to an API error code. */
export function handleApiError<T>(error: unknown): ApiResponse<T> {
  return { error: toApiErrorCode(error), data: null };
}
