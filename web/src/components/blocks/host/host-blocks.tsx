import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { HostServerSection } from '@/components/servers/host-server-section';
import { StarLayers } from '@/components/sky/star-layers';
import { env } from '@/config/server-env';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { wikiPathOf } from '@/lib/site-paths';
import { createServerAction } from './create-server.action';

/**
 * The hosting page under its hero, on the sky of the inner pages: the form
 * that creates a server on the master, with no account, then its key, shown
 * once, and the lines of `server.cfg` that hold it.
 */
export function HostBlocks({ lang }: { lang: Locale }) {
  const copy = getDictionary(lang).pages.hostPage;

  return (
    <div className="relative isolate">
      <StarLayers className="-z-10" />
      <PageHero eyebrow={copy.eyebrow} title={copy.title} lede={copy.lede} />
      <PageBody compact>
        <HostServerSection
          createServer={createServerAction}
          masterUrl={env.PUBLIC_MASTER_URL}
          guideHref={wikiPathOf(lang, 'host')}
          labels={copy}
        />
      </PageBody>
    </div>
  );
}
