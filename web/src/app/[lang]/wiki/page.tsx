import { redirect } from 'next/navigation';
import { readLocale } from '@/i18n/read-locale';
import { wikiPathOf } from '@/lib/site-paths';

/** The wiki opens on its first article. */
export default async function WikiPage({ params }: PageProps<'/[lang]/wiki'>) {
  redirect(wikiPathOf(await readLocale(params), 'keys'));
}
