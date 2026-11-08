import { PlayerSlots, type Seat } from '@/components/game/player-slots';
import { Tag } from '@/components/primitives/status/tag';
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeaderCell,
  TableRow,
} from '@/components/primitives/surfaces/table';
import type { GameState } from '@/lib/rtype/servers';

/** A game of a server, its words already written. */
export type GameRow = {
  id: string;
  /** "Partie 1". */
  label: string;
  state: GameState;
  stateLabel: string;
  seats: readonly Seat[];
};

export type GamesTableLabels = {
  game: string;
  state: string;
  seats: string;
  free: string;
};

/** The games of a server: their state and their seats. Read only: a seat is taken from the game. */
export function GamesTable({
  rows,
  labels,
}: {
  rows: readonly GameRow[];
  labels: GamesTableLabels;
}) {
  return (
    <Table data-slot="games-table">
      <TableHead>
        <TableRow>
          <TableHeaderCell>{labels.game}</TableHeaderCell>
          <TableHeaderCell>{labels.state}</TableHeaderCell>
          <TableHeaderCell>{labels.seats}</TableHeaderCell>
        </TableRow>
      </TableHead>
      <TableBody>
        {rows.map(row => (
          <TableRow key={row.id}>
            <TableCell className="text-star-white">{row.label}</TableCell>
            <TableCell>
              <Tag state={row.state}>{row.stateLabel}</Tag>
            </TableCell>
            <TableCell>
              <PlayerSlots
                seats={row.seats}
                size="sm"
                freeLabel={labels.free}
              />
            </TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}
