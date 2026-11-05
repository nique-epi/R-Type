import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import type { Health } from '../../schemas';

export async function getHealth(): Promise<ApiResponse<Health>> {
  const { data, error } = await master('@get/health');

  if (error) {
    return handleApiError<Health>(error);
  }

  return { data, error: null };
}
