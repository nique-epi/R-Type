import { CountUp } from '@/components/motion/count-up';
import { Stat } from '@/components/primitives/status/stat';
import type { PageCopy } from '@/i18n/page-copy';
import { freeSeatsOf, pilotsOf } from '@/lib/rtype/server-seats';
import type { MapServer } from '@/lib/rtype/servers';

/** The four figures of a server: its games, its pilots, its free seats, its version. */
export function ServerFigures({
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
