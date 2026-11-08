import type { Metadata } from 'next';

import { getDictionary } from './get-dictionary';
import { readLocale } from './read-locale';

/** The pages whose tab title is the title of their copy. */
type TitledPage = 'serversPage' | 'rankingPage' | 'teamPage' | 'downloadPage';

/** The metadata of a page titled by its copy, in the language of its path. */
export async function titleMetadata(
  params: Promise<{ lang: string }>,
  page: TitledPage,
): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages[page].title };
}
