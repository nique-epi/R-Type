import { render, screen } from '@testing-library/react';
import { describe, expect, it } from 'vitest';

import { VideoDialog } from './video-dialog';

function dialog(open: boolean) {
  render(
    <VideoDialog
      open={open}
      onClose={() => {}}
      title="Trailer"
      closeLabel="Close"
    >
      <p>soon</p>
    </VideoDialog>,
  );
  return screen.getByRole('dialog', { hidden: true });
}

describe('VideoDialog', () => {
  it('sets its grid layout only while it is open', () => {
    const classes = dialog(false).className.split(' ');

    expect(classes).toContain('open:grid');
    expect(classes).not.toContain('grid');
  });
});
