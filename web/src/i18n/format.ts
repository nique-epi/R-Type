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
 * The singular or the plural template of a count, by the plural rule of the
 * locale (French says "0 pilote", English "0 pilots"), left unfilled so a
 * caller can set the number in a node of its own.
 */
export function selectCountTemplate(
  count: number,
  templates: CountTemplates,
  locale: Locale,
): string {
  return pluralRulesOf(locale).select(count) === 'one'
    ? templates.one
    : templates.other;
}

/** Picks the template of a count with `selectCountTemplate()`, then fills it. */
export function formatCount(
  count: number,
  templates: CountTemplates,
  locale: Locale,
): string {
  return format(selectCountTemplate(count, templates, locale), { count });
}
