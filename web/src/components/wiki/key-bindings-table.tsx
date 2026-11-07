import { Keycap } from '@/components/primitives/keycap';
import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableHeaderCell,
  TableRow,
} from '@/components/primitives/table';

/** An action of the game and the keys that do it. */
export type KeyBinding = {
  action: string;
  keys: readonly string[];
};

/** A key this long is drawn on a wide cap, as the space bar. */
const WIDE_KEY_LENGTH = 4;

/** The commands of the game: the keys on caps, what they do beside them. */
export function KeyBindingsTable({
  bindings,
  labels,
}: {
  bindings: readonly KeyBinding[];
  labels: { key: string; action: string };
}) {
  return (
    <Table data-slot="key-bindings-table">
      <TableHead>
        <TableRow>
          <TableHeaderCell>{labels.key}</TableHeaderCell>
          <TableHeaderCell>{labels.action}</TableHeaderCell>
        </TableRow>
      </TableHead>
      <TableBody>
        {bindings.map(binding => (
          <TableRow key={binding.action}>
            <TableCell className="whitespace-nowrap">
              <span className="inline-flex gap-1.5">
                {binding.keys.map(key => (
                  <Keycap
                    key={key}
                    className={
                      key.length >= WIDE_KEY_LENGTH ? 'min-w-30' : undefined
                    }
                  >
                    {key}
                  </Keycap>
                ))}
              </span>
            </TableCell>
            <TableCell>{binding.action}</TableCell>
          </TableRow>
        ))}
      </TableBody>
    </Table>
  );
}
