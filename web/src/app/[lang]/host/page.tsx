import type { Metadata } from 'next';
import { HostBlocks } from '@/components/blocks/host/host-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/host'>): Promise<Metadata> {
  return titleMetadata(params, 'hostPage');
}

export default async function HostPage({ params }: PageProps<'/[lang]/host'>) {
  return <HostBlocks lang={await readLocale(params)} />;
}
