'use client';

import type * as React from 'react';
import { useEffect, useRef } from 'react';
import { useScrollLock } from '@/hooks/use-scroll-lock';
import { cn } from '@/lib/utils';

export type DialogProps = Omit<React.ComponentProps<'dialog'>, 'open'> & {
  open: boolean;
  /** Called on Escape, on a click on the backdrop, and by the close buttons. */
  onClose: () => void;
};

/**
 * A modal dialog on the native `<dialog>` element: the browser keeps the
 * focus inside it, closes it on Escape and gives the focus back to what
 * opened it. The page under it stops scrolling. Unstyled but for its
 * placement: the caller draws the box and its backdrop (`backdrop:` classes).
 */
export function Dialog({
  open,
  onClose,
  className,
  children,
  ref,
  ...props
}: DialogProps) {
  const dialog = useRef<HTMLDialogElement>(null);

  function attach(element: HTMLDialogElement | null) {
    dialog.current = element;
    if (typeof ref === 'function') {
      ref(element);
    } else if (ref) {
      ref.current = element;
    }
  }
  useScrollLock(open);

  useEffect(() => {
    const element = dialog.current;
    if (element === null) {
      return;
    }
    if (open && !element.open) {
      element.showModal();
    } else if (!open && element.open) {
      element.close();
    }
  }, [open]);

  function onCancel(event: React.SyntheticEvent<HTMLDialogElement>) {
    event.preventDefault();
    onClose();
  }

  function onClick(event: React.MouseEvent<HTMLDialogElement>) {
    if (event.target === event.currentTarget) {
      onClose();
    }
  }

  return (
    // biome-ignore lint/a11y/useKeyWithClickEvents: the native dialog closes on Escape through onCancel; the click only reads a click on the backdrop.
    <dialog
      ref={attach}
      data-slot="dialog"
      onCancel={onCancel}
      onClick={onClick}
      className={cn(
        'm-0 max-h-none max-w-none bg-transparent p-0 text-pale-blue open:fixed',
        className,
      )}
      {...props}
    >
      {children}
    </dialog>
  );
}
