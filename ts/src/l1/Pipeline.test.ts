// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Pipeline } from './Pipeline';
import { Transform } from './Transform';

interface Contact {
    name: string;
    email: string;
}

class TrimName implements Transform<Contact> {
    apply(contact: Contact): Contact {
        return { ...contact, name: contact.name.trim() };
    }
}

class LowercaseEmail implements Transform<Contact> {
    apply(contact: Contact): Contact {
        return { ...contact, email: contact.email.toLowerCase() };
    }
}

class TagDisplayName implements Transform<Contact> {
    constructor(private readonly suffix: string) {}

    apply(contact: Contact): Contact {
        return { ...contact, name: `${contact.name}${this.suffix}` };
    }
}

function contactNormalizer(): Pipeline<Contact> {
    const normalizer = new Pipeline<Contact>();
    normalizer.install(1, 'trim-name', new TrimName());
    normalizer.install(2, 'lowercase-email', new LowercaseEmail());
    return normalizer;
}

describe('Pipeline', () => {
    it('returns the same contact, unchanged, when no transforms are installed', () => {
        const normalizer = new Pipeline<Contact>();
        const contact: Contact = { name: 'Ada Lovelace', email: 'ada@example.com' };
        assert.equal(normalizer.apply(contact, {}), contact);
    });

    it('normalizes the name and email', () => {
        const normalizer = contactNormalizer();
        assert.deepEqual(
            normalizer.apply({ name: '  Ada Lovelace  ', email: 'ADA@EXAMPLE.COM' }, {}),
            { name: 'Ada Lovelace', email: 'ada@example.com' },
        );
    });

    it('runs transforms in priority order, not install order', () => {
        const applied: string[] = [];

        class TrackedTrimName implements Transform<Contact> {
            apply(contact: Contact): Contact {
                applied.push('trim-name');
                return contact;
            }
        }

        class TrackedLowercaseEmail implements Transform<Contact> {
            apply(contact: Contact): Contact {
                applied.push('lowercase-email');
                return contact;
            }
        }

        const normalizer = new Pipeline<Contact>();
        normalizer.install(2, 'lowercase-email', new TrackedLowercaseEmail());
        normalizer.install(1, 'trim-name', new TrackedTrimName());
        normalizer.apply({ name: 'Ada', email: 'ada@example.com' }, {});
        assert.deepEqual(applied, ['trim-name', 'lowercase-email']);
    });

    it('throws when two transforms share a priority', () => {
        const normalizer = new Pipeline<Contact>();
        normalizer.install(1, 'trim-name', new TrimName());
        assert.throws(() => {
            normalizer.install(1, 'duplicate', new LowercaseEmail());
        }, /Duplicate priority 1/);
    });

    it('only runs a transform when handles() approves the params', () => {
        // The transform owns its own gate: it knows normalization is opt-in,
        // so it declares that as part of itself.
        type NormalizeParams = { normalizeEmail: boolean };

        class OptInLowercaseEmail implements Transform<Contact, NormalizeParams> {
            handles(params: NormalizeParams): boolean {
                return params.normalizeEmail;
            }
            apply(contact: Contact): Contact {
                return { ...contact, email: contact.email.toLowerCase() };
            }
        }

        const normalizer = new Pipeline<Contact, NormalizeParams>();
        normalizer.install(1, 'lowercase-email', new OptInLowercaseEmail());
        const contact: Contact = { name: 'Ada', email: 'ADA@EXAMPLE.COM' };
        assert.deepEqual(normalizer.apply(contact, { normalizeEmail: false }), contact);
        assert.deepEqual(
            normalizer.apply(contact, { normalizeEmail: true }),
            { name: 'Ada', email: 'ada@example.com' },
        );
    });

    it('skips a transform whose rejects() rejects the current value', () => {
        class LowercaseEmailUnlessNoReply implements Transform<Contact> {
            rejects(contact: Contact): boolean {
                return contact.email.startsWith('NOREPLY');
            }
            apply(contact: Contact): Contact {
                return { ...contact, email: contact.email.toLowerCase() };
            }
        }

        const normalizer = new Pipeline<Contact>();
        normalizer.install(1, 'lowercase-email', new LowercaseEmailUnlessNoReply());
        assert.deepEqual(
            normalizer.apply({ name: 'System', email: 'NOREPLY@EXAMPLE.COM' }, {}),
            { name: 'System', email: 'NOREPLY@EXAMPLE.COM' },
        );
        assert.deepEqual(
            normalizer.apply({ name: 'Ada', email: 'ADA@EXAMPLE.COM' }, {}),
            { name: 'Ada', email: 'ada@example.com' },
        );
    });

    it('skips a transform whose accepts() rejects the current value', () => {
        class TrimNonEmptyName implements Transform<Contact> {
            accepts(contact: Contact): boolean {
                return contact.name.length > 0;
            }
            apply(contact: Contact): Contact {
                return { ...contact, name: contact.name.trim() };
            }
        }

        const normalizer = new Pipeline<Contact>();
        normalizer.install(1, 'trim-name', new TrimNonEmptyName());
        assert.deepEqual(
            normalizer.apply({ name: '', email: 'ada@example.com' }, {}),
            { name: '', email: 'ada@example.com' },
        );
        assert.deepEqual(
            normalizer.apply({ name: '  Ada  ', email: 'ada@example.com' }, {}),
            { name: 'Ada', email: 'ada@example.com' },
        );
    });

    it('evaluates accepts() against the transformed value, not the original', () => {
        class MaskInternalDomain implements Transform<Contact> {
            accepts(contact: Contact): boolean {
                return contact.email.endsWith('@example.com');
            }
            apply(contact: Contact): Contact {
                return { ...contact, email: 'redacted@example.com' };
            }
        }

        const normalizer = new Pipeline<Contact>();
        normalizer.install(1, 'lowercase-email', new LowercaseEmail());
        normalizer.install(2, 'mask-internal-domain', new MaskInternalDomain());
        // The raw input is uppercase, so mask-internal-domain's accepts() only
        // matches because lowercase-email already ran on it.
        assert.deepEqual(
            normalizer.apply({ name: 'Ada', email: 'ADA@EXAMPLE.COM' }, {}),
            { name: 'Ada', email: 'redacted@example.com' },
        );
    });

    it('installNew constructs a transform from its constructor arguments', () => {
        const normalizer = new Pipeline<Contact>();
        normalizer.installNew(1, 'tag-display-name', TagDisplayName, ' (verified)');
        assert.deepEqual(
            normalizer.apply({ name: 'Ada', email: 'ada@example.com' }, {}),
            { name: 'Ada (verified)', email: 'ada@example.com' },
        );
    });

    it('passes params through to apply', () => {
        type TagParams = { tag: string };

        class TagWithParam implements Transform<Contact, TagParams> {
            apply(contact: Contact, params: TagParams): Contact {
                return { ...contact, name: `${contact.name} (${params.tag})` };
            }
        }

        const normalizer = new Pipeline<Contact, TagParams>();
        normalizer.install(1, 'tag-display-name', new TagWithParam());
        assert.deepEqual(
            normalizer.apply({ name: 'Ada', email: 'ada@example.com' }, { tag: 'verified' }),
            { name: 'Ada (verified)', email: 'ada@example.com' },
        );
    });

    it('also works with primitive values, not just objects', () => {
        class ClampMinimumPage implements Transform<number> {
            apply(page: number): number {
                return Math.max(page, 1);
            }
        }

        const clampPage = new Pipeline<number>();
        clampPage.install(1, 'clamp-minimum', new ClampMinimumPage());
        assert.equal(clampPage.apply(0, {}), 1);
        assert.equal(clampPage.apply(5, {}), 5);
    });

    it('lets a transform declared without P run inside a pipeline typed with a specific P', () => {
        type NormalizeParams = { normalizeEmail: boolean };
        const normalizer = new Pipeline<Contact, NormalizeParams>();
        normalizer.install(1, 'trim-name', new TrimName());
        normalizer.install(2, 'lowercase-email', new LowercaseEmail());
        assert.deepEqual(
            normalizer.apply({ name: '  Ada  ', email: 'ADA@EXAMPLE.COM' }, { normalizeEmail: true }),
            { name: 'Ada', email: 'ada@example.com' },
        );
    });
});
