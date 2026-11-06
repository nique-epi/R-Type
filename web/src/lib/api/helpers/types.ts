import type { MasterErrorCode } from '../clients';

type ClientErrorCode =
  | 'ERR_API_FETCH_FAILED'
  | 'ERR_API_VALIDATION_FAILED'
  | 'UNKNOWN_ERROR';

export type ApiErrorCode = MasterErrorCode | ClientErrorCode;

export type ApiResponse<T> =
  | {
      error: null;
      data: T;
    }
  | {
      error: ApiErrorCode;
      data: null;
    };
