import type * as React from 'react';
import { cn } from '@/lib/utils';

/** A round button holding one icon; its name is required, the icon says nothing. */
export function IconButton({
  className,
  type = 'button',
  ...props
}: React.ComponentProps<'button'> & { 'aria-label': string }) {
  return (
    <button
      type={type}
      data-slot="icon-button"
      className={cn(
        'inline-grid size-12 shrink-0 cursor-pointer place-items-center rounded-full text-star-white transition-colors duration-200 hover:bg-pale-blue/14 focus-visible:outline-offset-3',
        className,
      )}
      {...props}
    />
  );
}
