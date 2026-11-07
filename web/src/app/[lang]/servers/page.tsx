import type { Metadata } from 'next';
import { connection } from 'next/server';
import { SectorMapBlocks } from '@/components/blocks/servers/sector-map-blocks';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/servers'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.serversPage.title };
}

export default async function ServersPage({
  params,
}: PageProps<'/[lang]/servers'>) {
  await connection();
  return <SectorMapBlocks lang={await readLocale(params)} />;
}
