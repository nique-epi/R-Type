// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';
import mermaid from 'astro-mermaid';
import starlightLinksValidator from 'starlight-links-validator';

export default defineConfig({
  site: 'https://nique-epi.github.io',
  base: '/R-Type',
  integrations: [
    mermaid({ enableLog: false }),
    starlight({
      title: 'R-Type',
      description:
        "Networked multiplayer remake of the R-Type shoot'em up, built on a custom C++ game engine.",
      logo: { src: './src/assets/logo.png', alt: 'R-Type', replacesTitle: true },
      customCss: ['./src/styles/theme.css'],
      social: [
        {
          icon: 'external',
          label: 'rtype.app',
          href: 'https://rtype.app',
        },
        {
          icon: 'github',
          label: 'GitHub',
          href: 'https://github.com/nique-epi/R-Type',
        },
      ],
      editLink: {
        baseUrl: 'https://github.com/nique-epi/R-Type/edit/main/docs/',
      },
      sidebar: [
        {
          label: 'Getting started',
          items: [
            'getting-started/download',
            'getting-started/build',
            'getting-started/play',
            'getting-started/test',
          ],
        },
        {
          label: 'Understand the project',
          items: [
            'overview',
            'architecture',
            'project-layout',
            'technology-choices',
            'modules',
            'concurrency',
          ],
        },
        {
          label: 'Subsystems',
          items: [
            'engine',
            'client',
            'server',
            'master',
            'network',
            'protocol',
            'logging',
            'accessibility',
          ],
        },
        {
          label: 'Modding and content',
          items: [
            'scripting',
            'waves-file',
            'entity-sheets',
            'tutorial-add-an-enemy-type',
            'tutorial-publish-a-mod',
          ],
        },
        {
          label: 'Tutorials',
          items: [
            'tutorial-write-a-module',
            'tutorial-add-a-network-message',
            'tutorial-run-several-games',
          ],
        },
        {
          label: 'Reference',
          items: ['commands', 'measurements', 'api-reference'],
        },
        {
          label: 'Operations',
          items: ['deploy-master', 'deploy-game-servers'],
        },
      ],
      head: [
        {
          // A table or a code block wider than the page scrolls; the keyboard
          // reaches it only if it can take the focus.
          tag: 'script',
          content: `
            const makeScrollablesFocusable = () => {
              document
                .querySelectorAll('.sl-markdown-content table, .expressive-code pre')
                .forEach((element) => {
                  if (element.scrollWidth > element.clientWidth) {
                    element.tabIndex = 0;
                  } else {
                    element.removeAttribute('tabindex');
                  }
                });
            };
            addEventListener('DOMContentLoaded', makeScrollablesFocusable);
            addEventListener('load', makeScrollablesFocusable);
            addEventListener('resize', makeScrollablesFocusable);`,
        },
      ],
      // The Doxygen reference is copied from public/api/ and is not a page.
      plugins: [starlightLinksValidator({ exclude: ['/R-Type/api/**'] })],
    }),
  ],
});
