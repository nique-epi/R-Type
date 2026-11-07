import type { Metadata } from 'next';
import Link from 'next/link';
import { notFound } from 'next/navigation';
import { connection } from 'next/server';
import type { Seat } from '@/components/game/player-slots';
import { PageBody } from '@/components/layout/page-body';
import { PageHero } from '@/components/layout/page-hero';
import { CountUp } from '@/components/motion/count-up';
import { Notice } from '@/components/primitives/status/notice';
import { Stat } from '@/components/primitives/status/stat';
import { StatusBadge } from '@/components/primitives/status/status-badge';
import { Heading } from '@/components/primitives/text/typography';
import { GamesTable } from '@/components/servers/games-table';
import { SamplesChart } from '@/components/servers/samples-chart/samples-chart';
import { getServerSheet } from '@/hooks/api';
import { format } from '@/i18n/format';
import { formatDuration } from '@/i18n/format-duration';
import { getDictionary } from '@/i18n/get-dictionary';
import type { PageCopy } from '@/i18n/page-copy';
import { readLocale } from '@/i18n/read-locale';
import { withoutAccents } from '@/lib/rtype/pixel-text';
import { MAXIMUM_PILOTS_PER_GAME, PLAYER_NUMBERS } from '@/lib/rtype/players';
import { freeSeatsOf, pilotsOf } from '@/lib/rtype/server-seats';
import type { MapServer } from '@/lib/rtype/servers';
import { pathOf } from '@/lib/site-paths';

/** The value at the top of the chart when no server is busier than this. */
const MINIMUM_CHART_MAXIMUM = 4;

export async function generateMetadata({
  params,
}: PageProps<'/[lang]/servers/[id]'>): Promise<Metadata> {
  await connection();
  const lang = await readLocale(params);
  const { id } = await params;
  const sheet = await getServerSheet(id);

  return {
    title:
      sheet?.server?.name ??
      format(getDictionary(lang).pages.serverPage.pendingName, { number: id }),
  };
}

/** The four seats of a game, the first `pilots` of them taken. */
function seatsOf(pilots: number, pilotLabel: string): Seat[] {
  return PLAYER_NUMBERS.map(player =>
    player <= pilots
      ? { player, title: format(pilotLabel, { number: player }) }
      : null,
  );
}

function gameRowsOf(server: MapServer, copy: ReturnType<typeof getDictionary>) {
  const { serverPage } = copy.pages;
  return server.games.map((game, index) => ({
    id: `game-${index}`,
    label: format(serverPage.gameName, { number: index + 1 }),
    state: game.state,
    stateLabel: copy.gameState[game.state],
    seats: seatsOf(
      Math.min(game.pilots, MAXIMUM_PILOTS_PER_GAME),
      serverPage.pilotCount,
    ),
  }));
}

export default async function ServerPage({
  params,
}: PageProps<'/[lang]/servers/[id]'>) {
  await connection();
  const lang = await readLocale(params);
  const { id } = await params;
  const sheet = await getServerSheet(id);

  if (!sheet) {
    notFound();
  }

  const dictionary = getDictionary(lang);
  const copy = dictionary.pages.serverPage;
  const { server } = sheet;
  const name = server?.name ?? format(copy.pendingName, { number: id });
  const since = format(copy.since, {
    duration: formatDuration(sheet.span, {
      seconds: copy.durationSeconds,
      minutes: copy.durationMinutes,
      hours: copy.durationHours,
    }),
  });

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
        {server ? (
          <section className="grid gap-4">
            <Heading size="section">{copy.gamesTitle}</Heading>
            {server.games.length > 0 ? (
              <GamesTable
                rows={gameRowsOf(server, dictionary)}
                labels={dictionary.gamesTable}
              />
            ) : (
              <p className="m-0">{copy.noGames}</p>
            )}
          </section>
        ) : null}
        {sheet.pilots.length > 0 ? (
          <section className="grid gap-4">
            <Heading size="section">
              {sheet.source === 'sample'
                ? copy.chartTitleSample
                : copy.chartTitle}
            </Heading>
            <SamplesChart
              samples={sheet.pilots}
              maximum={Math.max(MINIMUM_CHART_MAXIMUM, ...sheet.pilots)}
              startLabel={since}
              endLabel={dictionary.chart.now}
              aria-label={format(copy.chartAria, { name, since })}
            />
          </section>
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

/** The four figures of a server: its games, its pilots, its free seats, its version. */
function ServerFigures({
  copy,
  server,
}: {
  copy: PageCopy['serverPage'];
  server: MapServer;
}) {
  return (
    <div className="grid grid-cols-[repeat(auto-fit,minmax(200px,1fr))] gap-6">
      <Stat
        value={<CountUp value={server.games.length} delay={0.3} />}
        label={copy.games}
      />
      <Stat
        value={<CountUp value={pilotsOf(server)} delay={0.4} />}
        label={copy.pilots}
      />
      <Stat
        value={<CountUp value={freeSeatsOf(server)} delay={0.5} />}
        label={copy.free}
      />
      <Stat value={server.version} label={copy.version} />
    </div>
  );
}
