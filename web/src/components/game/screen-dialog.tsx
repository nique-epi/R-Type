import { cn } from '@/lib/utils';
import { CutFrame, type CutFrameProps } from './cut-frame';

export type ScreenDialogProps = CutFrameProps & { 'aria-label': string };

/**
 * A dialog laid inside a game screen, as the client lays one: the screen
 * dims under it, and the dialog is a cut frame over everything else on the
 * screen. The caller places it and sizes it (`className`, `style`).
 */
export function ScreenDialog({ className, ...props }: ScreenDialogProps) {
  return (
    <>
      <div aria-hidden="true" className="absolute inset-0 z-5 bg-sky/72" />
      <CutFrame
        role="dialog"
        data-slot="screen-dialog"
        className={cn('absolute z-6', className)}
        {...props}
      />
    </>
  );
}
