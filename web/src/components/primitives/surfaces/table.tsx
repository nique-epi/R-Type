import type * as React from 'react';
import { cn } from '@/lib/utils';

/** A table on the sunk ground, in a box that scrolls sideways on a phone. */
function Table({ className, ...props }: React.ComponentProps<'table'>) {
  return (
    <div data-slot="table-scroll" className="overflow-x-auto">
      <table
        data-slot="table"
        className={cn(
          'w-full border-collapse overflow-hidden rounded-lg bg-ink/60 text-sm leading-[22px]',
          className,
        )}
        {...props}
      />
    </div>
  );
}

function TableHead({ className, ...props }: React.ComponentProps<'thead'>) {
  return <thead data-slot="table-head" className={className} {...props} />;
}

function TableBody({ className, ...props }: React.ComponentProps<'tbody'>) {
  return (
    <tbody
      data-slot="table-body"
      className={cn('[&>tr:hover>td]:bg-deep-blue', className)}
      {...props}
    />
  );
}

function TableRow({ className, ...props }: React.ComponentProps<'tr'>) {
  return <tr data-slot="table-row" className={className} {...props} />;
}

function TableHeaderCell({
  className,
  scope = 'col',
  ...props
}: React.ComponentProps<'th'>) {
  return (
    <th
      data-slot="table-header-cell"
      scope={scope}
      className={cn(
        'whitespace-nowrap bg-deep-blue/70 px-4 py-3 text-left font-normal font-pixel text-hud-cyan text-xs uppercase leading-3',
        className,
      )}
      {...props}
    />
  );
}

function TableCell({
  className,
  numeric = false,
  ...props
}: React.ComponentProps<'td'> & {
  /** A figure: pixel digits, aligned on the right. */
  numeric?: boolean;
}) {
  return (
    <td
      data-slot="table-cell"
      className={cn(
        'border-deep-blue/60 border-t-2 px-4 py-3 align-middle text-pale-blue',
        numeric &&
          'whitespace-nowrap text-right font-pixel text-star-white text-xs',
        className,
      )}
      {...props}
    />
  );
}

export { Table, TableBody, TableCell, TableHead, TableHeaderCell, TableRow };
