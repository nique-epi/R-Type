import Link from 'next/link';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { Notice } from '@/components/primitives/status/notice';
import { StatusBadge } from '@/components/primitives/status/status-badge';
import type { ServerSheet } from '@/hooks/api';
import { format } from '@/i18n/format';
import { getDictionary } from '@/i18n/get-dictionary';
import type { Locale } from '@/i18n/locales';
import { withoutAccents } from '@/lib/rtype/pixel-text';
import { pathOf } from '@/lib/site-paths';
import { ServerChart } from './server-chart';
import { ServerFigures } from './server-figures';
import { ServerGames } from './server-games';

/** The page of one server under its hero: its figures, its games and its chart. */
export function ServerSheetBlocks({
  lang,
  id,
  sheet,
}: {
  lang: Locale;
  /** The id in the path, which names a server that has not registered yet. */
  id: string;
  sheet: ServerSheet;
}) {
  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.serverPage;
  const { server } = sheet;
  const name = server?.name ?? format(copy.pendingName, { number: id });

  return (
    <>
      <PageHero
        eyebrow={
          <>
            <Link
              href={pathOf(lang, 'servers')}
              className="text-hud-cyan no-underline hover:text-star-white"
            >
              {copy.back}
            </Link>{' '}
            <span className="text-pale-blue">/ {name}</span>
          </>
        }
        title={withoutAccents(name)}
      >
        {server ? (
          <StatusBadge status={server.status}>
            {dictionary.status[server.status]}
          </StatusBadge>
        ) : null}
      </PageHero>
      <PageBody>
        {server ? (
          <ServerFigures copy={copy} server={server} />
        ) : (
          <Notice title={copy.pendingTitle}>{copy.pending}</Notice>
        )}
        {server?.status === 'offline' ? (
          <Notice tone="warning" title={copy.offlineTitle}>
            {copy.offline}
          </Notice>
        ) : null}
        {server ? <ServerGames lang={lang} server={server} /> : null}
        {sheet.pilots.length > 0 ? (
          <ServerChart
            lang={lang}
            name={name}
            pilots={sheet.pilots}
            span={sheet.span}
            example={sheet.source === 'sample'}
          />
        ) : null}
        {sheet.source === 'sample' ? (
          <p className="m-0 small-caps">
            {dictionary.pages.site.sampleServers}
          </p>
        ) : null}
      </PageBody>
    </>
  );
}
