import { PROFESSOR_FRAME, SHEETS } from './sheets';
import { Sprite, type SpriteProps } from './sprite';

/** The professor of the help screens of the client. */
export function Professor({
  scale = 6,
  ...props
}: Omit<SpriteProps, 'sheet' | 'frame' | 'scale'> & { scale?: number }) {
  return (
    <Sprite
      sheet={SHEETS.professor}
      frame={PROFESSOR_FRAME}
      scale={scale}
      {...props}
    />
  );
}
