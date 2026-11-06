import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { MasterStatusText } from './master-status';

describe('MasterStatusText', () => {
  it('writes the version of a master that answers', () => {
    render(<MasterStatusText health={{ state: 'online', version: '0.1.0' }} />);

    expect(screen.getByText('Online, version 0.1.0')).toBeDefined();
  });

  it('writes that a silent master is unreachable', () => {
    render(<MasterStatusText health={{ state: 'unreachable' }} />);

    expect(screen.getByText('Unreachable')).toBeDefined();
  });
});
