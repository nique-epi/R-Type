import type * as React from 'react';
import { cn } from '@/lib/utils';

/** A band of the home page, with the space the mockup leaves around it. */
export function Section({
  className,
  ...props
}: React.ComponentProps<'section'>) {
  return (
    <section
      data-slot="section"
      className={cn('py-30 max-md:py-18', className)}
      {...props}
    />
  );
}
