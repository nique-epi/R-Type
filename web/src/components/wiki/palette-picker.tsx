'use client';

import { useId } from 'react';
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
              'relative flex w-full cursor-pointer flex-wrap items-center gap-4 py-2.5 pl-[22px] transition-colors duration-200 hover:text-star-white has-focus-visible:outline-2 has-focus-visible:outline-focus-ring has-focus-visible:outline-offset-2',
              chosen ? 'text-hud-cyan' : 'text-pale-blue',
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
            <span
              aria-hidden="true"
              className={cn(
                'absolute top-1/2 left-0 -mt-1 size-2 bg-hud-cyan transition-transform duration-200',
                chosen ? 'scale-100' : 'scale-0',
              )}
            />
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
