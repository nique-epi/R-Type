import { CodeBlock } from '@/components/primitives/text/code-block';
import { cn } from '@/lib/utils';

export type InstallStep = {
  title: string;
  /** A sentence to read, or a command to copy, or both. */
  text?: string;
  command?: string;
};

export type InstallStepsProps = {
  steps: readonly InstallStep[];
  /** The index of the step the reader is at, drawn with a white edge. */
  current?: number;
};

/** The steps to install the client, numbered in large pixel figures. */
export function InstallSteps({ steps, current }: InstallStepsProps) {
  return (
    <ol
      data-slot="install-steps"
      className="m-0 grid list-none grid-cols-[repeat(auto-fit,minmax(240px,1fr))] gap-6 p-0"
    >
      {steps.map((step, index) => {
        const active = index === current;
        return (
          <li
            key={step.title}
            aria-current={active ? 'step' : undefined}
            className={cn(
              'relative grid content-start gap-3 rounded-xl bg-deep-blue/30 p-7',
              active && 'shadow-[inset_0_0_0_2px_var(--color-star-white)]',
            )}
          >
            <span
              aria-hidden="true"
              className={cn(
                'font-pixel text-[32px] leading-8',
                active ? 'text-star-white' : 'text-deep-blue',
              )}
            >
              {index + 1}
            </span>
            <h3 className="m-0 font-pixel text-base text-hud-cyan uppercase leading-6">
              {step.title}
            </h3>
            {step.text ? <p className="m-0">{step.text}</p> : null}
            {step.command ? <CodeBlock>{step.command}</CodeBlock> : null}
          </li>
        );
      })}
    </ol>
  );
}
