import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

const keycapVariants = cva(
  'box-border inline-flex items-center justify-center bg-ink font-pixel text-star-white text-xs uppercase leading-3',
  {
    variants: {
      size: {
        /** The key of a binding in a table of the wiki. */
        md: 'h-9 min-w-10 rounded-lg border-2 border-pale-blue border-b-6 border-b-blue px-2',
        /** The key printed on a chip of a game screen. */
        sm: 'h-6 min-w-7 border-2 border-pale-blue border-b-4 border-b-blue px-1.5',
      },
    },
    defaultVariants: { size: 'md' },
  },
);

/** A key of the keyboard, lit from above as in the options of the client. */
export function Keycap({
  className,
  size,
  ...props
}: React.ComponentProps<'kbd'> & VariantProps<typeof keycapVariants>) {
  return (
    <kbd
      data-slot="keycap"
      className={cn(keycapVariants({ size }), className)}
      {...props}
    />
  );
}
