import { master } from '../../clients';
import { type ApiResponse, handleApiError } from '../../helpers';
import { type ListedServer, MAXIMUM_PAGE_SIZE } from '../../schemas';

/** The pages read at most, as the game client reads them (`MAXIMUM_LISTED_SERVER_PAGES`). */
const MAXIMUM_PAGES = 10;

/** Reads every page of `GET /api/servers`, up to `MAXIMUM_PAGES` of `MAXIMUM_PAGE_SIZE` servers. */
export async function listServers(): Promise<ApiResponse<ListedServer[]>> {
  const servers: ListedServer[] = [];

  for (let page = 0; page < MAXIMUM_PAGES; page += 1) {
    const { data, error } = await master('@get/servers', {
      query: { limit: MAXIMUM_PAGE_SIZE, offset: page * MAXIMUM_PAGE_SIZE },
    });

    if (error) {
      return handleApiError<ListedServer[]>(error);
    }

    servers.push(...data.servers);

    if (data.servers.length < MAXIMUM_PAGE_SIZE) {
      break;
    }
  }

  return { data: servers, error: null };
}
