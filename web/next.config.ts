import type { NextConfig } from 'next';
import { PHASE_DEVELOPMENT_SERVER } from 'next/constants';

import { parseEnv } from './src/config/env.config';
import { MASTER_API_PREFIX } from './src/config/master.config';

const API_PATHS = `${MASTER_API_PREFIX}/:path*`;

const sharedConfig: NextConfig = {
  agentRules: false,
  reactCompiler: true,
};

/** In development, the master API is proxied; the build is a static export. */
export default function nextConfig(phase: string): NextConfig {
  if (phase === PHASE_DEVELOPMENT_SERVER) {
    const { MASTER_URL } = parseEnv();

    return {
      ...sharedConfig,
      rewrites: async () => [
        { source: API_PATHS, destination: `${MASTER_URL}${API_PATHS}` },
      ],
    };
  }

  return {
    ...sharedConfig,
    output: 'export',
    trailingSlash: true,
    images: { unoptimized: true },
  };
}
