'use client';

import { useLayoutEffect, useRef, useState } from 'react';
import {
  TextField,
  type TextFieldProps,
} from '@/components/primitives/inputs/text-field';
import { typedUserCodeOf, userCodeCaretOf } from '@/lib/rtype/device-link';

export type UserCodeFieldProps = Omit<
  TextFieldProps,
  'value' | 'defaultValue' | 'onChange' | 'ref'
> & {
  /** The code the page opened with, shown as the field writes a code. */
  defaultValue: string;
};

/**
 * The field the page `/link` asks the code of the game in: it writes the code
 * as the game shows it while the player types, in capitals, the dash put in
 * after the fourth letter. Rewriting the value would send the caret to the
 * end, so the field puts it back after the letters that stood before it.
 */
export function UserCodeField({ defaultValue, ...props }: UserCodeFieldProps) {
  const [code, setCode] = useState(() => typedUserCodeOf(defaultValue, ''));
  const input = useRef<HTMLInputElement>(null);
  const caret = useRef<number | null>(null);

  useLayoutEffect(() => {
    if (caret.current !== null) {
      input.current?.setSelectionRange(caret.current, caret.current);
      caret.current = null;
    }
  });

  return (
    <TextField
      {...props}
      ref={input}
      value={code}
      onChange={event => {
        const typed = event.target.value;
        const shown = typedUserCodeOf(typed, code);
        caret.current = userCodeCaretOf(
          typed,
          event.target.selectionStart ?? typed.length,
          shown,
        );
        setCode(shown);
      }}
    />
  );
}
