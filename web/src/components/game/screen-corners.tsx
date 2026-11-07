import { cn } from '@/lib/utils';

const CORNERS = [
  'top-0 left-0 border-t-2 border-l-2',
  'top-0 right-0 border-t-2 border-r-2',
  'bottom-0 left-0 border-b-2 border-l-2',
  'right-0 bottom-0 border-r-2 border-b-2',
] as const;

/** The four brackets of the client around a box, as around the squadron of the title screen. */
export function ScreenCorners({ className }: { className?: string }) {
  return (
    <>
      {CORNERS.map(corner => (
        <span
          key={corner}
          aria-hidden="true"
          className={cn(
            'pointer-events-none absolute size-4 border-edge-blue',
            corner,
            className,
          )}
        />
      ))}
    </>
  );
}
