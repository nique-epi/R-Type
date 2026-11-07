import type * as React from 'react';
import { Fragment } from 'react';

/**
 * Fills the `{name}` holes of a template with React nodes, so a word of a
 * translated sentence can be set in bold or as a link without cutting the
 * sentence in pieces: format "Le serveur {name} est créé." with
 * `{ name: <strong>Nebula Lyon</strong> }`. A hole without a value stays as written.
 */
export function formatNodes(
  template: string,
  values: Readonly<Record<string, React.ReactNode>>,
): React.ReactNode {
  return template.split(/(\{\w+\})/).map((part, index) => {
    const name = /^\{(\w+)\}$/.exec(part)?.[1];
    const value = name === undefined ? part : (values[name] ?? part);
    // biome-ignore lint/suspicious/noArrayIndexKey: the parts of a sentence keep their order.
    return <Fragment key={index}>{value}</Fragment>;
  });
}
