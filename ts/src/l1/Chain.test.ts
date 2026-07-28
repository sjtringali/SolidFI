// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Chain } from './Chain';
import { Converter } from './Converter';

interface Contact {
    name: string;
    email: string;
}

class NamedEmailFormat implements Converter<string, Contact | null> {
    resolve(line: string): Contact | null {
        const match = /^(.+?)\s*<([^<>\s]+@[^<>\s]+)>$/.exec(line.trim());
        return match ? { name: match[1], email: match[2] } : null;
    }
}

class BareEmailFormat implements Converter<string, Contact | null> {
    resolve(line: string): Contact | null {
        const email = line.trim();
        return /^[^<>\s]+@[^<>\s]+$/.test(email) ? { name: email.split('@')[0], email } : null;
    }
}

class CompanyEmailFormat implements Converter<string, Contact | null> {
    constructor(private readonly domain: string) {}

    resolve(line: string): Contact | null {
        const email = line.trim();
        return email.endsWith(`@${this.domain}`) ? { name: email.split('@')[0], email } : null;
    }
}

function contactParser(): Chain<string, Contact | null> {
    const parser = new Chain<string, Contact | null>(null);
    parser.install(1, 'named-email', new NamedEmailFormat());
    parser.install(2, 'bare-email', new BareEmailFormat());
    return parser;
}

describe('Chain', () => {
    it('resolves with the first matching format', () => {
        const parser = contactParser();
        assert.deepEqual(
            parser.resolve('Ada Lovelace <ada@example.com>', {}),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('falls through to the next format when the first does not match', () => {
        const parser = contactParser();
        assert.deepEqual(
            parser.resolve('ada@example.com', {}),
            { name: 'ada', email: 'ada@example.com' },
        );
    });

    it('returns the chain failure value when nothing matches', () => {
        const parser = contactParser();
        assert.equal(parser.resolve('not a contact', {}), null);
    });

    it('tries formats in priority order, not install order', () => {
        const tried: string[] = [];

        class TrackedNamedEmailFormat implements Converter<string, Contact | null> {
            resolve(): Contact | null {
                tried.push('named-email');
                return null;
            }
        }

        class TrackedBareEmailFormat implements Converter<string, Contact | null> {
            resolve(): Contact | null {
                tried.push('bare-email');
                return null;
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        parser.install(2, 'bare-email', new TrackedBareEmailFormat());
        parser.install(1, 'named-email', new TrackedNamedEmailFormat());
        parser.resolve('ada@example.com', {});
        assert.deepEqual(tried, ['named-email', 'bare-email']);
    });

    it('throws when two formats share a priority', () => {
        const parser = new Chain<string, Contact | null>(null);
        parser.install(1, 'named-email', new NamedEmailFormat());
        assert.throws(() => {
            parser.install(1, 'duplicate', new BareEmailFormat());
        }, /Duplicate priority 1/);
    });

    it('only tries a format when handles() approves the params', () => {
        // The format owns its own gate: it knows it's a legacy, opt-in format,
        // so it declares that as part of itself rather than relying on the
        // caller to conditionally install it.
        type ParseParams = { allowLegacyFormat: boolean };

        class LegacyPipeSeparatedFormat implements Converter<string, Contact | null, ParseParams> {
            handles(params: ParseParams): boolean {
                return params.allowLegacyFormat;
            }
            resolve(line: string): Contact | null {
                const [name, email] = line.split('|');
                return name && email ? { name, email } : null;
            }
        }

        const parser = new Chain<string, Contact | null, ParseParams>(null);
        parser.install(1, 'legacy-pipe-separated', new LegacyPipeSeparatedFormat());
        assert.equal(parser.resolve('Ada Lovelace|ada@example.com', { allowLegacyFormat: false }), null);
        assert.deepEqual(
            parser.resolve('Ada Lovelace|ada@example.com', { allowLegacyFormat: true }),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('skips a format whose rejects() rejects the line', () => {
        class NamedEmailOnlyFormat implements Converter<string, Contact | null> {
            rejects(line: string): boolean {
                return !line.includes('<');
            }
            resolve(line: string): Contact | null {
                const match = /^(.+?)\s*<([^<>\s]+@[^<>\s]+)>$/.exec(line.trim());
                return match ? { name: match[1], email: match[2] } : null;
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        parser.install(1, 'named-email', new NamedEmailOnlyFormat());
        assert.equal(parser.resolve('ada@example.com', {}), null);
        assert.deepEqual(
            parser.resolve('Ada Lovelace <ada@example.com>', {}),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('skips a format whose accepts() rejects the line', () => {
        class NamedEmailOnlyFormat implements Converter<string, Contact | null> {
            accepts(line: string): boolean {
                return line.includes('<');
            }
            resolve(line: string): Contact | null {
                const match = /^(.+?)\s*<([^<>\s]+@[^<>\s]+)>$/.exec(line.trim());
                return match ? { name: match[1], email: match[2] } : null;
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        parser.install(1, 'named-email', new NamedEmailOnlyFormat());
        assert.equal(parser.resolve('ada@example.com', {}), null);
        assert.deepEqual(
            parser.resolve('Ada Lovelace <ada@example.com>', {}),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('accepts a blank object as a per-format failure sentinel', () => {
        // GUIDE.md calls this out directly: null, undefined, a Symbol, or "a blank
        // object" are all equally valid failure markers. This adapts an existing
        // format's null-based failure convention to a Contact-shaped placeholder.
        const UNRECOGNIZED: Contact = { name: '', email: '' };

        class NamedEmailFormatWithSentinel implements Converter<string, Contact | null> {
            resolve(line: string): Contact | null {
                return new NamedEmailFormat().resolve(line) ?? UNRECOGNIZED;
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        parser.install(1, 'named-email', new NamedEmailFormatWithSentinel(), UNRECOGNIZED);
        parser.install(2, 'bare-email', new BareEmailFormat());
        assert.deepEqual(
            parser.resolve('ada@example.com', {}),
            { name: 'ada', email: 'ada@example.com' },
        );
    });

    it('treats a result equal to the chain default as success when the format has its own sentinel', () => {
        const UNRECOGNIZED: Contact = { name: '', email: '' };
        const tried: string[] = [];

        class LiteralNullFormat implements Converter<string, Contact | null> {
            resolve(): Contact | null {
                tried.push('literal-null-contact');
                return null;
            }
        }

        class TrackedBareEmailFormat implements Converter<string, Contact | null> {
            resolve(line: string): Contact | null {
                tried.push('bare-email');
                return new BareEmailFormat().resolve(line);
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        // This format's own failure marker is UNRECOGNIZED, not null, so
        // returning null here is a legitimate success, even though it matches
        // the chain's overall failure value.
        parser.install(1, 'literal-null-contact', new LiteralNullFormat(), UNRECOGNIZED);
        parser.install(2, 'bare-email', new TrackedBareEmailFormat());
        assert.equal(parser.resolve('ada@example.com', {}), null);
        assert.deepEqual(tried, ['literal-null-contact']);
    });

    it('checks failure by identity, not by structural equality', () => {
        const UNRECOGNIZED: Contact = { name: '', email: '' };
        const tried: string[] = [];

        class CopycatFormat implements Converter<string, Contact | null> {
            resolve(): Contact | null {
                tried.push('copycat');
                // Same shape as UNRECOGNIZED, but a different reference.
                return { name: '', email: '' };
            }
        }

        class TrackedBareEmailFormat implements Converter<string, Contact | null> {
            resolve(line: string): Contact | null {
                tried.push('bare-email');
                return new BareEmailFormat().resolve(line);
            }
        }

        const parser = new Chain<string, Contact | null>(null);
        // Chain compares with `!==`, which is reference equality for objects, so
        // CopycatFormat's result counts as a genuine success, not a failure.
        parser.install(1, 'copycat', new CopycatFormat(), UNRECOGNIZED);
        parser.install(2, 'bare-email', new TrackedBareEmailFormat());
        assert.deepEqual(parser.resolve('ada@example.com', {}), { name: '', email: '' });
        assert.deepEqual(tried, ['copycat']);
    });

    it('accepts a Symbol as a sentinel, just as well as null or an object', () => {
        const UNPARSEABLE = Symbol('unparseable');

        class NamedEmailFormatWithSentinel implements Converter<string, Contact | typeof UNPARSEABLE> {
            resolve(line: string): Contact | typeof UNPARSEABLE {
                return new NamedEmailFormat().resolve(line) ?? UNPARSEABLE;
            }
        }

        class BareEmailFormatWithSentinel implements Converter<string, Contact | typeof UNPARSEABLE> {
            resolve(line: string): Contact | typeof UNPARSEABLE {
                return new BareEmailFormat().resolve(line) ?? UNPARSEABLE;
            }
        }

        const parser = new Chain<string, Contact | typeof UNPARSEABLE>(UNPARSEABLE);
        parser.install(1, 'named-email', new NamedEmailFormatWithSentinel());
        parser.install(2, 'bare-email', new BareEmailFormatWithSentinel());
        assert.deepEqual(
            parser.resolve('ada@example.com', {}),
            { name: 'ada', email: 'ada@example.com' },
        );
        assert.equal(parser.resolve('not a contact', {}), UNPARSEABLE);
    });

    it('installNew constructs a format from its constructor arguments', () => {
        const parser = new Chain<string, Contact | null>(null);
        parser.installNew(1, 'company-email', CompanyEmailFormat, 'acme.example');
        assert.deepEqual(
            parser.resolve('ada@acme.example', {}),
            { name: 'ada', email: 'ada@acme.example' },
        );
        assert.equal(parser.resolve('ada@example.com', {}), null);
    });

    it('passes params through to resolve', () => {
        type ParseParams = { requiredDomain: string };

        class DomainRestrictedEmailFormat implements Converter<string, Contact | null, ParseParams> {
            resolve(line: string, params: ParseParams): Contact | null {
                const email = line.trim();
                return email.endsWith(`@${params.requiredDomain}`) ? { name: email.split('@')[0], email } : null;
            }
        }

        const parser = new Chain<string, Contact | null, ParseParams>(null);
        parser.install(1, 'domain-restricted-email', new DomainRestrictedEmailFormat());
        assert.equal(parser.resolve('ada@example.com', { requiredDomain: 'acme.example' }), null);
        assert.deepEqual(
            parser.resolve('ada@acme.example', { requiredDomain: 'acme.example' }),
            { name: 'ada', email: 'ada@acme.example' },
        );
    });

    it('lets a format declared without P run inside a chain typed with a specific P', () => {
        type ParseParams = { allowBareEmail: boolean };
        const parser = new Chain<string, Contact | null, ParseParams>(null);
        parser.install(1, 'named-email', new NamedEmailFormat());
        parser.install(2, 'bare-email', new BareEmailFormat());
        assert.deepEqual(
            parser.resolve('Ada Lovelace <ada@example.com>', { allowBareEmail: false }),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('combines formats declared for different Parameters shapes, as long as they match structurally', () => {
        type AllowBareEmail = { allowBareEmail: boolean };
        type RequiredDomain = { requiredDomain: string };
        type ParseParams = AllowBareEmail & RequiredDomain;

        class OptInBareEmailFormat implements Converter<string, Contact | null, AllowBareEmail> {
            handles(params: AllowBareEmail): boolean {
                return params.allowBareEmail;
            }
            resolve(line: string): Contact | null {
                const email = line.trim();
                return /^[^<>\s]+@[^<>\s]+$/.test(email) ? { name: email.split('@')[0], email } : null;
            }
        }

        class DomainRestrictedEmailFormat implements Converter<string, Contact | null, RequiredDomain> {
            resolve(line: string, params: RequiredDomain): Contact | null {
                const email = line.trim();
                return email.endsWith(`@${params.requiredDomain}`) ? { name: email.split('@')[0], email } : null;
            }
        }

        const parser = new Chain<string, Contact | null, ParseParams>(null);
        parser.install(1, 'domain-restricted-email', new DomainRestrictedEmailFormat());
        parser.install(2, 'opt-in-bare-email', new OptInBareEmailFormat());

        // domain-restricted-email only reads requiredDomain, opt-in-bare-email only
        // reads allowBareEmail; ParseParams satisfies both because it's a superset of each.
        assert.deepEqual(
            parser.resolve('ada@acme.example', { requiredDomain: 'acme.example', allowBareEmail: false }),
            { name: 'ada', email: 'ada@acme.example' },
        );
        assert.deepEqual(
            parser.resolve('ada@example.com', { requiredDomain: 'acme.example', allowBareEmail: true }),
            { name: 'ada', email: 'ada@example.com' },
        );
    });

    it('supports a strict, closed Parameters shape instead of the default open bag', () => {
        interface AuthParams {
            readonly apiKey: string;
        }

        class AuthenticatedBareEmailFormat implements Converter<string, Contact | null, AuthParams> {
            resolve(line: string, params: AuthParams): Contact | null {
                // apiKey is required, not optional, so no guard is needed here.
                if (params.apiKey.length === 0) {
                    return null;
                }
                const email = line.trim();
                return /^[^<>\s]+@[^<>\s]+$/.test(email) ? { name: email.split('@')[0], email } : null;
            }
        }

        const parser = new Chain<string, Contact | null, AuthParams>(null);
        parser.install(1, 'authenticated-bare-email', new AuthenticatedBareEmailFormat());
        assert.deepEqual(
            parser.resolve('ada@example.com', { apiKey: 'secret' }),
            { name: 'ada', email: 'ada@example.com' },
        );
        assert.equal(parser.resolve('ada@example.com', { apiKey: '' }), null);
    });
});
