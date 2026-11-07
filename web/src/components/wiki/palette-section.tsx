'use client';

import { useState } from 'react';
import { format } from '@/i18n/format';
import { PalettePicker, type PalettePickerProps } from './palette-picker';

export type PaletteSectionProps = Pick<
  PalettePickerProps,
  'palettes' | 'labels'
> & {
  /** "Palette choisie : {name}". */
  chosen: string;
};

/**
 * The palettes of the pilots, the first of them picked at first, and the name
 * of the one picked. The pick is
 * only shown here: the palette of the game is set in its own options.
 */
export function PaletteSection({
  palettes,
  labels,
  chosen,
}: PaletteSectionProps) {
  const [value, setValue] = useState(palettes[0]?.id ?? '');
  const name = palettes.find(palette => palette.id === value)?.label ?? '';

  return (
    <>
      <PalettePicker
        palettes={palettes}
        value={value}
        onValueChange={setValue}
        labels={labels}
      />
      <p role="status" className="m-0 small-caps text-star-white">
        {format(chosen, { name })}
      </p>
    </>
  );
}
