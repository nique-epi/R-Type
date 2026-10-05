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
      social: [
        {
          icon: 'github',
          label: 'GitHub',
          href: 'https://github.com/nique-epi/R-Type',
        },
      ],
      editLink: {
        baseUrl: 'https://github.com/nique-epi/R-Type/edit/main/docs/',
      },
      plugins: [starlightLinksValidator()],
    }),
  ],
});
