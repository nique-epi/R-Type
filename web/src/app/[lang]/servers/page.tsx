import type { Metadata } from 'next';
import { connection } from 'next/server';
import { SectorMapBlocks } from '@/components/blocks/servers/sector-map-blocks';
import { titleMetadata } from '@/i18n/page-metadata';
import { readLocale } from '@/i18n/read-locale';

export function generateMetadata({
  params,
}: PageProps<'/[lang]/servers'>): Promise<Metadata> {
  return titleMetadata(params, 'serversPage');
}

export default async function ServersPage({
  params,
}: PageProps<'/[lang]/servers'>) {
  await connection();
  return <SectorMapBlocks lang={await readLocale(params)} />;
}
