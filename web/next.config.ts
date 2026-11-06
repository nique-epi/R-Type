import type { NextConfig } from 'next';

import { parseEnv } from './src/config/env.config';

parseEnv();

const nextConfig: NextConfig = {
  agentRules: false,
  reactCompiler: true,
};

export default nextConfig;
