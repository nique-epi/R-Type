import type { Metadata } from 'next';
import { HostServerBlocks } from '@/components/blocks/servers/host-server-blocks';
import { getDictionary } from '@/i18n/get-dictionary';
import { readLocale } from '@/i18n/read-locale';
import { createServerAction } from './actions';

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/servers/new'>): Promise<Metadata> {
  const lang = await readLocale(params);
  return { title: getDictionary(lang).pages.hostPage.title };
}

export default async function HostServerPage({
  params,
}: PageProps<'/[lang]/servers/new'>) {
  return (
    <HostServerBlocks
      lang={await readLocale(params)}
      create={createServerAction}
    />
  );
}
