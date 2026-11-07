import { PlayerSlots, type Seat } from '@/components/game/player-slots';
import { Button } from '@/components/primitives/actions/button';
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
  /** The word of the button: "Rejoindre", or "Votre place". */
  actionLabel: string;
  'aria-label': string;
  /** Joins the game; a game that cannot be joined has none, and its button is off. */
  onJoin?: () => void;
};

export type GamesTableLabels = {
  game: string;
  state: string;
  seats: string;
  action: string;
  free: string;
};

/** The games of a server: their state, their seats, and the button to join one. */
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
          <TableHeaderCell>
            <span className="sr-only">{labels.action}</span>
          </TableHeaderCell>
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
            <TableCell className="text-right">
              <Button
                size="sm"
                disabled={row.onJoin === undefined}
                onClick={row.onJoin}
                aria-label={row['aria-label']}
              >
                {row.actionLabel}
              </Button>
            </TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}
