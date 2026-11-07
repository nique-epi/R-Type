import { cva } from 'class-variance-authority';
import { cn } from '@/lib/utils';

/**
 * A row of a side list (the articles of the wiki, the palettes): room on its
 * left for the marker, pale blue, cyan when it is the one shown or chosen.
 */
export const markedRowVariants = cva(
  'relative flex w-full items-center py-2.5 pl-[22px] transition-colors duration-200 hover:text-star-white',
  {
    variants: {
      marked: { true: 'text-hud-cyan', false: 'text-pale-blue' },
    },
    defaultVariants: { marked: false },
  },
);

/** The cyan square before the row shown, which grows in when the row is chosen. */
export function SideMarker({ shown }: { shown: boolean }) {
  return (
    <span
      aria-hidden="true"
      data-slot="side-marker"
      className={cn(
        'absolute top-1/2 left-0 -mt-1 size-2 bg-hud-cyan transition-transform duration-200',
        shown ? 'scale-100' : 'scale-0',
      )}
    />
  );
}
