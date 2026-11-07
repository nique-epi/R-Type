import { render } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { formatNodes } from './format-nodes';

describe('formatNodes', () => {
  it('sets a node in the hole and keeps the rest of the sentence around it', () => {
    const { container } = render(
      <p>
        {formatNodes('Le serveur {name} est créé.', {
          name: <strong>Nebula</strong>,
        })}
      </p>,
    );

    expect(container.textContent).toBe('Le serveur Nebula est créé.');
    expect(container.querySelector('strong')?.textContent).toBe('Nebula');
  });

  it('leaves a hole without a value as written', () => {
    const { container } = render(<p>{formatNodes('dans le {file}', {})}</p>);

    expect(container.textContent).toBe('dans le {file}');
  });
});
