import type { Metadata } from 'next';
import { TeamBlocks } from '@/components/blocks/team/team-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/team'>): Promise<Metadata> {
  return titleMetadata(params, 'teamPage');
}

export default async function TeamPage({ params }: PageProps<'/[lang]/team'>) {
  return <TeamBlocks lang={await readLocale(params)} />;
}
