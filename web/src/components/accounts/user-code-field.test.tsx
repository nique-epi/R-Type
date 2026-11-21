import { fireEvent, render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { UserCodeField } from './user-code-field';

const LABEL = 'Code';

function renderField(defaultValue = '') {
  render(
    <UserCodeField label={LABEL} name="code" defaultValue={defaultValue} />,
  );
  return screen.getByLabelText(LABEL) as HTMLInputElement;
}

function type(field: HTMLInputElement, value: string) {
  fireEvent.change(field, { target: { value } });
}

describe('UserCodeField', () => {
  it('puts the dash in once the fourth letter is typed', () => {
    const field = renderField();

    type(field, 'xcm');
    type(field, 'xcmh');

    expect(field.value).toBe('XCMH-');
  });

  it('lets the player erase the dash it put in', () => {
    const field = renderField();

    type(field, 'xcmh');
    type(field, 'XCMH');

    expect(field.value).toBe('XCMH');
  });

  it('keeps the caret where the player types inside the code', () => {
    const field = renderField('XCMH-CXCZ');

    fireEvent.change(field, {
      target: { value: 'kMH-CXCZ', selectionStart: 1, selectionEnd: 1 },
    });
    fireEvent.change(field, {
      target: { value: 'KqMHC-XCZ', selectionStart: 2, selectionEnd: 2 },
    });

    expect(field.value).toBe('KQMH-CXCZ');
    expect(field.selectionStart).toBe(2);
  });

  it('writes the code the page opened with as the game shows it', () => {
    expect(renderField('xcmhcxcz').value).toBe('XCMH-CXCZ');
  });
});
