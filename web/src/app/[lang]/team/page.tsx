import type { Metadata } from 'next';
import { TeamBlocks } from '@/components/blocks/team/team-blocks';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/team'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.teamPage.title };
}

export default async function TeamPage({ params }: PageProps<'/[lang]/team'>) {
  return <TeamBlocks lang={await readLocale(params)} />;
}
