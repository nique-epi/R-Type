'use client';

import { useId } from 'react';
import {
  markedRowVariants,
  SideMarker,
} from '@/components/primitives/icons/side-marker';
import { format } from '@/i18n/format';
import { cn } from '@/lib/utils';

/** A palette of the four pilots, as the accessibility options of the client offer them. */
export type PilotPalette = {
  id: string;
  label: string;
  /** The colors of pilots 1 to 4. */
  colors: readonly [string, string, string, string];
};

export type PalettePickerProps = {
  palettes: readonly PilotPalette[];
  value: string;
  onValueChange: (id: string) => void;
  labels: {
    group: string;
    /** "Pilote {number}". */
    pilot: string;
  };
};

/**
 * The palettes of the pilots as native radio buttons, a row each with its
 * four colors beside its name: the arrows move the choice and the browser
 * announces it, with no script of its own.
 */
export function PalettePicker({
  palettes,
  value,
  onValueChange,
  labels,
}: PalettePickerProps) {
  const name = useId();

  return (
    <fieldset
      data-slot="palette-picker"
      className="m-0 grid min-w-0 gap-3 border-0 p-0"
    >
      <legend className="sr-only">{labels.group}</legend>
      {palettes.map(palette => {
        const chosen = palette.id === value;
        return (
          <label
            key={palette.id}
            className={cn(
              markedRowVariants({ marked: chosen }),
              'cursor-pointer flex-wrap gap-4 has-focus-visible:outline-2 has-focus-visible:outline-focus-ring has-focus-visible:outline-offset-2',
            )}
          >
            <input
              type="radio"
              name={name}
              value={palette.id}
              checked={chosen}
              onChange={() => onValueChange(palette.id)}
              className="sr-only"
            />
            <SideMarker shown={chosen} />
            <span className="w-40 small-caps">{palette.label}</span>
            {palette.colors.map((color, pilot) => (
              <span
                key={color}
                title={format(labels.pilot, { number: pilot + 1 })}
                className="inline-block h-6 w-12"
                style={{ background: color }}
              />
            ))}
          </label>
        );
      })}
    </fieldset>
  );
}
