import type { InstallStep } from '@/components/download/install-steps';
import type { PageCopy } from '@/i18n/page-copy';
import type { System } from './release';

type StepWords = PageCopy['downloadPage']['steps'];

/**
 * The three steps that install the client on a system, from the file of its
 * release: Linux makes the AppImage executable then runs it, the others
 * open an installer and start the game from their menu.
 */
export function installStepsFor(
  system: System,
  words: StepWords,
  file: string,
): InstallStep[] {
  const download = { title: words.download };

  if (system === 'linux') {
    return [
      download,
      { title: words.makeExecutable, command: `chmod +x ${file}` },
      { title: words.launch, command: `./${file}` },
    ];
  }

  return system === 'windows'
    ? [
        download,
        { title: words.install, text: words.installWindows },
        { title: words.launch, text: words.launchWindows },
      ]
    : [
        download,
        { title: words.install, text: words.installMacos },
        { title: words.launch, text: words.launchMacos },
      ];
}
