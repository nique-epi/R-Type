import { CutFrame } from '@/components/game/cut-frame';
import { GameText } from '@/components/game/game-text';
import { cn } from '@/lib/utils';

/** A game in the panel, its words already written. */
export type PanelGame = {
  name: string;
  /** "3/4". */
  pilots: string;
  state: string;
  joinable: boolean;
  'aria-label': string;
};

export type SectorGamesPanelProps = {
  title: string;
  'aria-label': string;
  columns: { game: string; pilots: string; state: string };
  games: readonly PanelGame[];
  chosen: number;
  onPick: (index: number) => void;
  /** What Enter does with the chosen game, under the list. */
  description: string;
  /** The description is the reader's own seat, written in white; otherwise a hint in cyan or a plain line. */
  descriptionTone: 'you' | 'hint' | 'plain';
  noGames: string;
};

/** The list shows at most six games, as the panel of the client. */
export const PANEL_GAMES = 6;
const FIRST_ROW_TOP = 70;
const ROW_STEP = 30;

const DESCRIPTION_TONES = {
  you: 'text-star-white',
  hint: 'text-hud-cyan',
  plain: 'text-pale-blue',
} as const;

/** The games of the chosen server, over the dimmed map: a row each, the chosen one lit. */
export function SectorGamesPanel({
  title,
  'aria-label': label,
  columns,
  games,
  chosen,
  onPick,
  description,
  descriptionTone,
  noGames,
}: SectorGamesPanelProps) {
  return (
    <>
      <div aria-hidden="true" className="absolute inset-0 z-4 bg-sky/72" />
      <CutFrame
        data-games-panel
        role="dialog"
        aria-label={label}
        className="absolute top-0 left-80 z-4 h-84 w-136 bg-hud-cyan"
      >
        <GameText className="absolute top-2.5 left-[18px] text-hud-cyan">
          {title}
        </GameText>
        <span className="absolute top-[34px] left-[18px] h-0.5 w-126 bg-deep-blue" />
        {games.length === 0 ? (
          <GameText className="absolute inset-x-0 top-[74px] text-center text-pale-blue">
            {noGames}
          </GameText>
        ) : (
          <>
            <GameText
              face="small"
              className="absolute top-[46px] left-[22px] text-pale-blue"
            >
              {columns.game}
            </GameText>
            <GameText
              face="small"
              className="absolute top-[46px] left-[238px] text-pale-blue"
            >
              {columns.pilots}
            </GameText>
            <GameText
              face="small"
              className="absolute top-[46px] left-[350px] text-pale-blue"
            >
              {columns.state}
            </GameText>
            {games.slice(0, PANEL_GAMES).map((game, index) => {
              const selected = index === chosen;
              return (
                <button
                  // biome-ignore lint/suspicious/noArrayIndexKey: a game is listed by its place on its server.
                  key={index}
                  type="button"
                  data-game-row
                  aria-label={game['aria-label']}
                  aria-pressed={selected}
                  onClick={() => onPick(index)}
                  className={cn(
                    'absolute left-3.5 h-[26px] w-128 cursor-pointer text-left focus-visible:outline-offset-0',
                    selected ? 'bg-edge-blue text-ink' : 'text-pale-blue',
                  )}
                  style={{ top: FIRST_ROW_TOP + index * ROW_STEP }}
                >
                  <GameText as="span" className="absolute top-[5px] left-2">
                    {game.name}
                  </GameText>
                  <GameText
                    as="span"
                    className={cn(
                      'absolute top-[5px] left-56',
                      selected ? 'text-ink' : 'text-star-white',
                    )}
                  >
                    {game.pilots}
                  </GameText>
                  <GameText
                    as="span"
                    face="small"
                    className={cn(
                      'absolute top-[3px] left-84 bg-ink px-1.5 py-[3px]',
                      game.joinable ? 'text-hud-cyan' : 'text-star-white',
                    )}
                  >
                    {game.state}
                  </GameText>
                </button>
              );
            })}
            <GameText
              face="small"
              role="status"
              className={cn(
                'absolute inset-x-[18px] top-[298px]',
                DESCRIPTION_TONES[descriptionTone],
              )}
            >
              {description}
            </GameText>
          </>
        )}
      </CutFrame>
    </>
  );
}
