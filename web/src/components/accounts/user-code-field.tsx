'use client';

import { useState } from 'react';
import {
  TextField,
  type TextFieldProps,
} from '@/components/primitives/inputs/text-field';
import { typedUserCodeOf } from '@/lib/rtype/device-link';

export type UserCodeFieldProps = Omit<
  TextFieldProps,
  'value' | 'defaultValue' | 'onChange'
> & {
  /** The code the page opened with, shown as the field writes a code. */
  defaultValue: string;
};

/**
 * The field the page `/link` asks the code of the game in: it writes the code
 * as the game shows it while the player types, in capitals, the dash put in
 * after the fourth letter.
 */
export function UserCodeField({ defaultValue, ...props }: UserCodeFieldProps) {
  const [code, setCode] = useState(() => typedUserCodeOf(defaultValue, ''));

  return (
    <TextField
      {...props}
      value={code}
      onChange={event => {
        const typed = event.target.value;
        setCode(previous => typedUserCodeOf(typed, previous));
      }}
    />
  );
}
