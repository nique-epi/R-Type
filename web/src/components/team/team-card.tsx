import { Float } from '@/components/motion/float';
import { Ship } from '@/components/sprites/ship';
import type { PlayerNumber } from '@/lib/rtype/players';

export type TeamCardProps = {
  name: string;
  url: string;
  /** The seat whose ship the member flies, 1 to 4. */
  player: PlayerNumber;
  /** "GitHub {handle}", already written. */
  linkLabel: string;
};

/** A member of the team: their ship hovering, their name, a link to their GitHub. */
export function TeamCard({ name, url, player, linkLabel }: TeamCardProps) {
  return (
    <article
      data-slot="team-card"
      className="grid justify-items-start gap-4 rounded-xl bg-deep-blue/35 p-7 transition duration-250 hover:-translate-y-1 hover:bg-deep-blue/60"
    >
      <Float distance={6} duration={1.2} delay={player * 0.2}>
        <Ship player={player} />
      </Float>
      <h2 className="m-0 font-pixel text-base text-star-white uppercase leading-6">
        {name}
      </h2>
      <a
        href={url}
        target="_blank"
        rel="noopener noreferrer"
        className="border-deep-blue border-b-2 pb-1 font-small text-base text-hud-cyan leading-4 no-underline transition-colors duration-200 hover:border-hud-cyan focus-visible:border-hud-cyan"
      >
        {linkLabel}
      </a>
    </article>
  );
}
