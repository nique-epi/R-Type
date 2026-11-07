import type { Metadata } from 'next';
import { connection } from 'next/server';
import { Notice } from '@/components/primitives/status/notice';
import { SectorMapScreen } from '@/components/servers/sector-map-screen';
import { getMappedServerList } from '@/hooks/api';
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
  const lang = await readLocale(params);
  const dictionary = getDictionary(lang);
  const { servers, source } = await getMappedServerList();

  return (
    <main className="starfield relative overflow-hidden border-edge-blue border-b-2">
      <h1 className="sr-only">{dictionary.pages.serversPage.title}</h1>
      <SectorMapScreen
        servers={servers}
        labels={dictionary.sectorMap}
        locale={lang}
      />
      {source === 'sample' ? (
        <Notice
          tone="warning"
          title={dictionary.pages.site.sampleTitle}
          className="relative mx-auto max-w-240 px-4 py-3"
        >
          {dictionary.pages.site.sampleServers}
        </Notice>
      ) : null}
    </main>
  );
}
