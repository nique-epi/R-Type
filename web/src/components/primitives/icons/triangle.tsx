import { cva, type VariantProps } from 'class-variance-authority';
import type * as React from 'react';
import { cn } from '@/lib/utils';

const triangleVariants = cva(
  'inline-block size-0 shrink-0 border-y-transparent border-l-current',
  {
    variants: {
      direction: {
        right: '',
        left: '-scale-x-100',
        down: 'rotate-90',
      },
      size: {
        sm: 'border-y-6 border-l-8',
        md: 'border-y-8 border-l-10',
      },
    },
    defaultVariants: { direction: 'right', size: 'sm' },
  },
);

/** The pixel arrow of the game menus, drawn in the current text color. */
export function Triangle({
  className,
  direction,
  size,
  ...props
}: React.ComponentProps<'span'> & VariantProps<typeof triangleVariants>) {
  return (
    <span
      aria-hidden="true"
      data-slot="triangle"
      className={cn(triangleVariants({ direction, size }), className)}
      {...props}
    />
  );
}
