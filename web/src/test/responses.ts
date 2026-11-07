/** A JSON response, as the tests stub `fetch` with it for the master and GitHub. */
export function answer(status: number, body: unknown): Response {
  return new Response(JSON.stringify(body), {
    status,
    headers: { 'Content-Type': 'application/json' },
  });
}

/** A server as `GET /api/servers` lists it. */
export function listedServer(id: number) {
  return {
    id,
    name: `Server ${id}`,
    gameVersion: '0.1.0',
    maximumGames: 8,
    maximumPlayersPerGame: 4,
    status: 'online',
    gameCount: 2,
    playerCount: 3,
  };
}
