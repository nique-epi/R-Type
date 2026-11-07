import { Notice } from '@/components/primitives/status/notice';
import { SectorMapScreen } from '@/components/servers/sector-map-screen';
import { getMappedServerList } from '@/hooks/api';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';

/** The sector map of the servers, full width, with a notice when they are examples. */
export async function SectorMapBlocks({ lang }: { lang: Locale }) {
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
