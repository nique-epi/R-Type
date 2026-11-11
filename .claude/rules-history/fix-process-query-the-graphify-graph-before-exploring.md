# RULE: Before exploring the code, query the graphify graph when one exists — then read only what it names

## Context

A feature gave an entity several collision boxes. Its first step was to find every place that reads the collision box of an entity, across the game library, the server and the client. The main checkout held a graphify graph of the repository (`graphify-out/`, ignored by git), built the day before.

## Mistake

The exploration started with whole files: the sheet reader, the sheet text reader and the box components in one command, 33 KB of output, before any question was put to the graph. The user stopped the turn and asked to use graphify, and to write this rule.

## Root cause

The graph was not looked for: the worktree of the session has no `graphify-out/` folder, since the folder is ignored by git and lives in the main checkout only.

## What the graph gave

One query with a budget of 2500 tokens named the collision system, the lag compensated hit system, the missiles the client predicts and the look of an entity type, each with its file and line. It did not list every reader: `git grep` found the others (the playfield exit, the pilot system, the weapon system), which is why point 4 keeps `git grep` for complete lists.

## Rule

1. Before the first exploration of a task, look for `graphify-out/graph.json` at the root of the main checkout.
2. When it exists, ask it first, with a budget.
3. Read only the files and the ranges it names.
4. Take complete lists from `git grep`, and check on disk what the graph names.
5. Never build or update the graph unasked.
