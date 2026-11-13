import type * as React from 'react';
import { useId } from 'react';
import { cn } from '@/lib/utils';

export type TextFieldProps = Omit<React.ComponentProps<'input'>, 'id'> & {
  label: React.ReactNode;
  /** Shown under the field and announced, when the value is refused. */
  error?: React.ReactNode;
  /** A line of help under the field, such as a count of characters. */
  help?: React.ReactNode;
};

/** A labelled text box sunk into the page; its label turns cyan while it is typed in. */
export function TextField({
  label,
  error,
  help,
  className,
  ...props
}: TextFieldProps) {
  const id = useId();
  const errorId = `${id}-error`;
  const helpId = `${id}-help`;
  const describedBy =
    [error ? errorId : null, help ? helpId : null].filter(Boolean).join(' ') ||
    undefined;

  return (
    <div data-slot="text-field" className={cn('group grid gap-2', className)}>
      <label
        htmlFor={id}
        className="small-caps text-pale-blue group-focus-within:text-hud-cyan"
      >
        {label}
      </label>
      <input
        id={id}
        aria-invalid={error ? true : undefined}
        aria-describedby={describedBy}
        className="box-border min-h-12 w-full rounded-2xl border-2 border-deep-blue bg-ink/80 px-[18px] py-3.5 font-pixel text-base text-pale-blue leading-4 caret-hud-cyan placeholder:text-pale-blue focus:border-hud-cyan focus:text-star-white focus:outline-none aria-invalid:border-warning-orange"
        {...props}
      />
      {error ? (
        <span
          id={errorId}
          role="alert"
          className="small-caps text-warning-orange"
        >
          {error}
        </span>
      ) : null}
      {help ? (
        <span id={helpId} className="text-pale-blue text-sm leading-[22px]">
          {help}
        </span>
      ) : null}
    </div>
  );
}
