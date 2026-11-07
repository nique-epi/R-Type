import type { Locale } from './locales';

/**
 * Fills the `{name}` holes of a template from a dictionary. Dictionaries hold
 * templates rather than functions, so a server component can hand them to a
 * client component. A hole without a value stays as written.
 *
 * @example format('{count} PILOTES', { count: 3 }) // '3 PILOTES'
 */
export function format(
  template: string,
  values: Readonly<Record<string, string | number>>,
): string {
  return template.replace(/\{(\w+)\}/g, (hole, name: string) => {
    const value = values[name];
    return value === undefined ? hole : String(value);
  });
}

const PLURAL_RULES = new Map<Locale, Intl.PluralRules>();

/** The plural rule of a locale, built once. */
function pluralRulesOf(locale: Locale): Intl.PluralRules {
  let rules = PLURAL_RULES.get(locale);
  if (rules === undefined) {
    rules = new Intl.PluralRules(locale);
    PLURAL_RULES.set(locale, rules);
  }
  return rules;
}

/** The singular and plural templates of a count, `{count}` as the number. */
export type CountTemplates = Readonly<{ one: string; other: string }>;

/**
 * Picks the singular or the plural template by the plural rule of the locale
 * (French says "0 pilote", English "0 pilots"), then fills it.
 */
export function formatCount(
  count: number,
  templates: CountTemplates,
  locale: Locale,
): string {
  const rule = pluralRulesOf(locale).select(count);
  return format(rule === 'one' ? templates.one : templates.other, { count });
}
